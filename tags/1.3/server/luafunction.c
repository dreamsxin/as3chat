#include "MyleftServer.h"

// lua 脚本里面的函数由C调用

void lua_init() {
    /* 初始化Lua */
    L = lua_open();
    /* 载入Lua基本库 */
    luaL_openlibs(L);
}

void lua_load_config() {
    /* 运行脚本 */
    int err = luaL_dofile(L, "conf/config.conf");
    if (err) {
        //如果错误，显示
        log_write(LOG_ERR, lua_tostring(L, -1), __FILE__, __LINE__);
        //弹出栈顶的这个错误信息
        lua_pop(L, 1);
        return;
    }
    /* 获取变量 */
    lua_getglobal(L, "port");
    if (lua_isnumber(L, -1)) {
        /* 得到结果 */
        port = (int) lua_tointeger(L, -1);
        printf("port:%d\n", port);
    }
    //弹出栈顶
    lua_pop(L, 1);

    lua_getglobal(L, "pthread");
    if (lua_isnumber(L, -1)) {
        t_min = (int) lua_tointeger(L, -1);
        printf("t_min:%d\n", t_min);
    }
    lua_pop(L, 1);

    lua_getglobal(L, "dbhost");
    if (lua_isstring(L, -1)) {
        strncpy(dbhost, lua_tostring(L, -1), MAX_CHAR_LENGTH);
    }
    lua_pop(L, 1);

    lua_getglobal(L, "dbport");
    if (lua_isnumber(L, -1)) {
        dbport = (int) lua_tointeger(L, -1);
    }
    lua_pop(L, 1);

    lua_getglobal(L, "dbuser");
    if (lua_isstring(L, -1)) {
        strncpy(dbuser, lua_tostring(L, -1), MAX_CHAR_LENGTH);
    }
    lua_pop(L, 1);

    lua_getglobal(L, "dbpass");
    if (lua_isstring(L, -1)) {
        strncpy(dbpass, lua_tostring(L, -1), MAX_CHAR_LENGTH);
    }
    lua_pop(L, 1);

    lua_getglobal(L, "dbname");
    if (lua_isstring(L, -1)) {
        strncpy(dbname, lua_tostring(L, -1), MAX_CHAR_LENGTH);
    }
    lua_pop(L, 1);

    lua_getglobal(L, "dbtable");
    if (lua_isstring(L, -1)) {
        strncpy(dbtable, lua_tostring(L, -1), MAX_CHAR_LENGTH);
    }
    lua_pop(L, 1);

    lua_getglobal(L, "fieldusername");
    if (lua_isstring(L, -1)) {
        strncpy(fieldusername, lua_tostring(L, -1), MAX_CHAR_LENGTH);
    }
    lua_pop(L, 1);

    lua_getglobal(L, "fieldpassword");
    if (lua_isstring(L, -1)) {
        strncpy(fieldpassword, lua_tostring(L, -1), MAX_CHAR_LENGTH);
    }
    lua_pop(L, 1);

    lua_getglobal(L, "fieldsalt");
    if (lua_isstring(L, -1)) {
        strncpy(fieldsalt, lua_tostring(L, -1), MAX_CHAR_LENGTH);
    }
    lua_pop(L, 1);
    
    lua_getglobal(L, "crossdomain");
    if (lua_isstring(L, -1)) {
        strncpy(crossdomain, lua_tostring(L, -1), MAX_BUFFER_LENGTH);
    }
    lua_pop(L, 1);

    printf("crossdomain:%s, file:%s, line:%i\n", crossdomain, __FILE__, __LINE__);
}

char *lua_load_key(const char *account) {
    /* 运行脚本 */
    int err = luaL_dofile(L, "conf/account.conf");
    if (err) {
        //如果错误，显示
        log_write(LOG_ERR, lua_tostring(L, -1), __FILE__, __LINE__);
        return NULL;
    }
    printf("lua_getglobal:%s\n", account);
    /* 获取变量 */
    lua_getglobal(L, account);
    printf("lua_getglobal2\n");
    // 获取参数个数
    //int n = lua_gettop(L);
    // printf("lua_gettop:%d\n", n);
    if (lua_isstring(L, -1)) {

        printf("lua_getglobal3\n");
        /* 得到结果 */
        //char temp[MAX_BUFFER_LENGTH];
        // strncpy( temp, lua_tostring( L, -1), MAX_CHAR_LENGTH);

        char *temp = (char *) malloc(MAX_CHAR_LENGTH * sizeof (char));
        strncpy(temp, lua_tostring(L, -1), MAX_CHAR_LENGTH);

        lua_pop(L, 1);
        printf("temp:%s\n", temp);
        return temp;
    }
    return NULL;
}

void lua_load_room() {
    /* 运行脚本 */
    int err = luaL_dofile(L, "conf/room.conf");
    if (err) {
        //如果错误，显示
        log_write(LOG_ERR, lua_tostring(L, -1), __FILE__, __LINE__);
        return;
    }

    /* 获取变量 */
    lua_getglobal(L, "rooms");

    int i;
    // 获取参数个数
    int n = lua_gettop(L);
    int len = lua_objlen(L, n); //下标从1开始的，如果从0开始，要+1
    printf("n:%d,num:%d\n", n, lua_objlen(L, n));
    printf("-n:%d,num:%d\n", n, lua_objlen(L, -1));
    if (lua_istable(L, -1)) {

        /*
                lua_pushnil(L); // nil 入栈作为初始 key
                while (0 != lua_next(L, -2)) {
                    // 现在栈顶（-1）是 value，-2 位置是对应的 key
                    // 这里可以判断 key 是什么并且对 value 进行各种处理
                    if (lua_isstring(L, -1)) {
                        rooms[i].enable = 1;
                        strncpy(rooms[i].name, lua_tostring(L, -1), MAX_CHAR_LENGTH);
                    }
                    lua_pop(L, 1); // 弹出 value，让 key 留在栈顶
                }
                // 现在栈顶是 table
         */
        for (i = 0; i < len && i < MAX_ROOMS; i++) {
            lua_rawgeti(L, 1, i);
            if (lua_isstring(L, -1)) {
                rooms[i].enable = 1;
                strncpy(rooms[i].name, lua_tostring(L, -1), MAX_CHAR_LENGTH);
            } else {
                break;
            }
            printf("n:%d, i:%d, %s\n", n, i, lua_tostring(L, -1));
        }
    }
}

int lua_get_type(const char *account) {
    int result = CLIENT_TYPE_NONE;
    /* 运行脚本 */
    int err = luaL_dofile(L, "conf/user.conf");
    if (err) {
        //如果错误，显示
        log_write(LOG_ERR, lua_tostring(L, -1), __FILE__, __LINE__);
        return result;
    }

    /* 获取变量 */
    lua_getglobal(L, "users");

    // 获取参数个数
    if (lua_istable(L, -1)) {
        lua_pushstring(L, account);
        lua_gettable(L, -2);
        if (lua_isnumber(L, -1)) {
            result = (int) lua_tonumber(L, -1);
        }
        lua_pop(L, 1);
    }
    return result;
}
