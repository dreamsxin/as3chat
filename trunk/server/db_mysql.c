#include "MyleftServer.h"
#define MYSQL_LU  256

void db_connect() {
    //数据库
    conn = malloc(sizeof (MYSQL));

    mysql_init(conn);
    if (conn == NULL) exit(EXIT_FAILURE);

    mysql_options(conn, MYSQL_SET_CHARSET_NAME, "utf8");
    char value = 1;
    mysql_options(conn, MYSQL_OPT_RECONNECT, (char *) & value);

    if (!mysql_real_connect(conn, dbhost, dbuser, dbpass, dbname, MYSQL_PORT, NULL, CLIENT_INTERACTIVE)) {
        log_write(LOG_ERR, mysql_error(conn), __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }
}

void db_close() {
    mysql_close(conn);
    free(conn);
}

MYSQL_RES *get_user_tuple(const char *username) {

    char iuser[MYSQL_LU + 1];
    char euser[MAX_CHAR_LENGTH * 2 + 1];
    printf("test1\n");
    if (mysql_ping(conn) != 0) {
        log_write(LOG_ERR, "mysql: connection to database lost", __FILE__, __LINE__);
        return NULL;
    }
    printf("test2\n");
    snprintf(iuser, MAX_CHAR_LENGTH, "%s", username);
    mysql_real_escape_string(conn, euser, iuser, strlen(iuser));

    printf("test4\n");

    char sql[256];
    snprintf(sql, sizeof (sql), "SELECT * FROM %s WHERE %s='%s' limit 1", dbtable, fieldusername, euser);
    log_write(LOG_ERR, sql, __FILE__, __LINE__);

    if (mysql_query(conn, sql) != 0) {

        log_write(LOG_ERR, mysql_error(conn), __FILE__, __LINE__);
        return NULL;
    }
    printf("test8\n");
    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL) {
        log_write(LOG_ERR, mysql_error(conn), __FILE__, __LINE__);
        return NULL;
    }
    printf("test9\n");
    if (mysql_num_rows(res) != 1) {
        mysql_free_result(res);
        return NULL;
    }
    
    printf("test end\n");
    return res;
}

int get_password(const char* username, char password[MAX_CHAR_LENGTH], char salt[MAX_CHAR_LENGTH]) {

    MYSQL_RES *res = get_user_tuple(username);
    MYSQL_FIELD *field;
    MYSQL_ROW row;
    printf("authenticate2\n");
    if (res == NULL) {
        return RETURN_FAILURE;
    }
    printf("authenticate3\n");
    int i, fpassword = 0, fsalt = 0;
    for (i = mysql_num_fields(res) - 1; i >= 0; i--) {
        field = mysql_fetch_field_direct(res, i);
        if (strcmp(field->name, fieldpassword) == 0) {
            fpassword = i;
        }
        if (strcmp(field->name, fieldsalt) == 0) {
            fsalt = i;
        }
    }
    if ((row = mysql_fetch_row(res)) == NULL) {
        printf("mysql: sql tuple retrieval failed: %s\n", mysql_error(conn));
        mysql_free_result(res);
        return RETURN_FAILURE;
    }

    if (row[fpassword] == NULL) {
        mysql_free_result(res);
        return RETURN_FAILURE;
    }

    strcpy(password, row[fpassword]);
    strcpy(salt, row[fsalt]);
    printf("password:%s, fieldsalt:%s, salt:%s, fsalt:%d\n", password, fieldsalt, salt, fsalt);
    mysql_free_result(res);
    return RETURN_SUCCESS;
}
