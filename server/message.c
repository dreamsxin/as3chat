#include "MyleftServer.h"

void *readtask(void *args) {
    while (1) {
        //互斥访问任务队列
        pthread_mutex_lock(&t_mutex);
        //等待到任务队列不为空
        while (task_head == NULL) {
            //线程阻塞，释放互斥锁，当等待的条件等到满足时，它会再次获得互斥锁
            pthread_cond_wait(&t_cond, &t_mutex);
        }

        //从任务队列取出一个读任务
        struct task *temp = task_head;
        task_head = task_head->next;
        pthread_mutex_unlock(&t_mutex);
        log_write(LOG_DEBUG, "readtask", __FILE__, __LINE__);

        parse_message(temp);
    }
}

// Parses the message and sends a response

void parse_message(struct task * args) {
    clients *fdnode = args->client;
    int fd = fdnode->fd;
    int logout = args->logout;
    char in[MAX_BUFFER_LENGTH];
    strcpy(in, args->data);
    free(args->data);
    free(args);


    char msgbuffer[MAX_BUFFER_LENGTH];

    printf("logout:%d,message:%s\n", logout, in);

    if (logout) {
        node_del(fdnode);
        return;
    }

    printf("parse_message:%s, file:%s, line:%i\n", in, __FILE__, __LINE__);
    log_write(LOG_DEBUG, "start parse_message", __FILE__, __LINE__);

    ezxml_t event = ezxml_parse_str(in, sizeof (in));
    if (!ezxml_attr(event, "type")) {
        goto end;
    }
    int ev_type = atoi(ezxml_attr(event, "type"));
    const char *username = ezxml_attr(event, "username");
    const char *password = ezxml_attr(event, "password");
    const char *anonymous = ezxml_attr(event, "anonymous");
    const char *account = ezxml_attr(event, "account");
    const char *sign = ezxml_attr(event, "sign");
    const char *timestamp_char = ezxml_attr(event, "timestamp");

    int roomid = 0;
    if (ezxml_attr(event, "roomid")) {
        roomid = atoi(ezxml_attr(event, "roomid"));
        roomid = roomid >= MAX_ROOMS ? 0 : roomid;
    }

    time_t timestamp = 0;
    if (timestamp_char) timestamp = atol(timestamp_char);

    const char *to = ezxml_attr(event, "to");
    const char *body;
    if (ezxml_child(event, "body")) body = ezxml_child(event, "body")->txt;
    //char *body = strescape(temp_body);

    time_t mytimestamp;
    struct tm *p;
    time(&mytimestamp);
    p = gmtime(&mytimestamp);

    printf("ev_type%d,file:%s, line:%d\n", ev_type, __FILE__, __LINE__);

    if (ev_type == EV_TYPE_AUTH) {
        log_write(LOG_DEBUG, "登陆验证", __FILE__, __LINE__);
        if (fdnode->state == FD_STATE_SUCCESS) {
            bzero(msgbuffer, sizeof (msgbuffer));
            snprintf(msgbuffer, sizeof (msgbuffer), "<event type='%d' result='1' message='已经登陆'/>", EV_TYPE_AUTH);
            send_message(fd, msgbuffer);
            goto end;
        } else if (anonymous && atoi(anonymous) == 1) {//匿名登陆
            log_write(LOG_DEBUG, "匿名登陆", __FILE__, __LINE__);
            fdnode->anonymous = 1;
            snprintf(fdnode->username, MAX_CHAR_LENGTH, "游客%d%d%d", p->tm_hour, p->tm_min, p->tm_sec);
        } else if (username && (password || (sign && account && timestamp))) {
            log_write(LOG_DEBUG, "判断验证", __FILE__, __LINE__);
            fdnode->anonymous = 0;
            strncpy(fdnode->username, username, MAX_CHAR_LENGTH);
            //判断是密码登陆还是加密接口登陆
            if (password) {
                //db获取用户密码
                char db_pw_value[MAX_CHAR_LENGTH];
                char db_salt_value[MAX_CHAR_LENGTH];

                if (get_password(username, db_pw_value, db_salt_value) != RETURN_SUCCESS) {
                    bzero(msgbuffer, sizeof (msgbuffer));
                    snprintf(msgbuffer, sizeof (msgbuffer), "<event type='%d' result='0' message='密码错误'/>", EV_TYPE_AUTH);
                    //write(fd, out, strlen(out));
                    send_message(fd, msgbuffer);
                    goto end;
                }

                //char password[MAX_CHAR_LENGTH] = "13145200";
                char buf[MAX_CHAR_LENGTH] = {'\0'};

                md5(password, buf);
                printf("%s\n", buf);

                char temp_password[MAX_CHAR_LENGTH];
                sprintf(temp_password, "%s%s", buf, db_salt_value);
                printf("temp_password:%s\n", temp_password);
                bzero(&buf, sizeof (buf));

                md5(temp_password, (char*) buf);
                if (strcmp(buf, db_pw_value) != 0) {
                    bzero(msgbuffer, sizeof (msgbuffer));
                    snprintf(msgbuffer, sizeof (msgbuffer), "<event type='%d' result='0' message='密码错误'/>", EV_TYPE_AUTH);
                    //write(fd, out, strlen(out));
                    send_message(fd, msgbuffer);
                    goto end;
                }
            } else {
                //判断是否过期
                long sec = mytimestamp - timestamp;
                if (sec >= 0 && sec < 60) {
                    char crypt_key[MAX_BUFFER_LENGTH];
                    char *temp_key = lua_load_key(account);
                    strncpy(crypt_key, temp_key, MAX_CHAR_LENGTH);
                    free(temp_key); //释放内存

                    char temp_str[MAX_BUFFER_LENGTH];
                    bzero(temp_str, sizeof (temp_str));
                    strncat(temp_str, crypt_key, 18);
                    strcat(temp_str, username);
                    strcat(temp_str, timestamp_char);

                    char crypt_str[MAX_CHAR_LENGTH];

                    strncpy(crypt_str, crypt(temp_str, sign), sizeof (crypt_str));
                    //判断加密串是否正确
                    if (strcmp(crypt_str, sign) != 0) {
                        bzero(msgbuffer, sizeof (msgbuffer));
                        snprintf(msgbuffer, sizeof (msgbuffer), "<event type='%d' result='0' message='验证码错误'/>", EV_TYPE_AUTH);
                        //write(fd, out, strlen(out));
                        send_message(fd, msgbuffer);
                        goto end;
                    }
                } else {
                    bzero(msgbuffer, sizeof (msgbuffer));
                    snprintf(msgbuffer, sizeof (msgbuffer), "<event type='%d' result='0' message='验证过期'/>", EV_TYPE_AUTH);
                    send_message(fd, msgbuffer);
                    goto end;
                }
            }
        } else {
            bzero(msgbuffer, sizeof (msgbuffer));
            snprintf(msgbuffer, sizeof (msgbuffer), "<event type='%d' result='0' message='请输入用户名和密码'/>", EV_TYPE_AUTH);
            send_message(fd, msgbuffer);
            goto end;
        }

        //判断该用户名的用户在其他地方是否有登陆
        clients *node = get_fdnode_byname(fdnode->username);
        if (node != NULL) {
            log_write(LOG_DEBUG, "存在同名用户", __FILE__, __LINE__);
            other_same_username(node);
        }

        log_write(LOG_DEBUG, "test--------------------------", __FILE__, __LINE__);

        hash_item *item = (hash_item *) malloc(sizeof (hash_item));
        item->key = fdnode->username;
        item->data = (void *) fdnode->fd;
        item->next = NULL;

        hash_add(item);

        fdnode->roomid = roomid;
        fdnode->type = lua_get_type(fdnode->username);
        fdnode->state = FD_STATE_SUCCESS;

        //发送登陆成功消息
        bzero(msgbuffer, sizeof (msgbuffer));
        snprintf(msgbuffer, sizeof (msgbuffer), "<event type='%d' result='1' username='%s' usertype='%d' message='登陆成功'/>", EV_TYPE_AUTH, fdnode->username, fdnode->type);
        send_message(fd, msgbuffer);

        //加入房间
        join_room(fdnode, roomid);

    }
    else if (ev_type == EV_TYPE_USER_CHANGE_STATE && fdnode->state == FD_STATE_SUCCESS) {
            int state = 0;
            if (!ezxml_attr(event, "state")) state = atoi(ezxml_attr(event, "state"));

            snprintf(msgbuffer, sizeof (msgbuffer), "<event type='%d' username='%s' state='%d'/>", EV_TYPE_USER_CHANGE_STATE, fdnode->username, state);
            send_message_all(fd, msgbuffer);
            goto end;
    }
    else if (ev_type == EV_TYPE_MESSAGE && fdnode->state == FD_STATE_SUCCESS && body) {

        bzero(msgbuffer, sizeof (msgbuffer));
        if (fdnode->type == CLIENT_TYPE_SHUTUP )
        {
            snprintf(msgbuffer, sizeof (msgbuffer), "<event type='%d' timestamp='%ld'><body><![CDATA[您目前处于禁言状态]]></body></event>", EV_TYPE_MESSAGE, mytimestamp);
            send_message(fd, msgbuffer);
            goto end;
        }
        if (!to || strncmp(to, ALL_USERS, sizeof (ALL_USERS)) == 0) {
            printf("timestamp:%ld", mytimestamp);
            snprintf(msgbuffer, sizeof (msgbuffer), "<event type='%d' from='%s' timestamp='%ld'><body><![CDATA[%s]]></body></event>", EV_TYPE_MESSAGE, fdnode->username, mytimestamp, body);
            send_message_all(fd, msgbuffer);
            goto end;
        } else {
            snprintf(msgbuffer, sizeof (msgbuffer), "<event type='%d' from='%s' to='%s' timestamp='%ld'><body><![CDATA[%s]]></body></event>", EV_TYPE_MESSAGE, fdnode->username, to, mytimestamp, body);

            if (strcmp(fdnode->username, to) == 0) {
                send_message(fd, msgbuffer);
            } else {
                clients *tonode = get_fdnode_byname(to);
                if (tonode != NULL && tonode->roomid == fdnode->roomid) {
                    send_message(tonode->fd, msgbuffer);
                    send_message(fd, msgbuffer);
                }
            }
            goto end;
        }
    } else if (ev_type == EV_TYPE_CHANGE_ROOM && fdnode->state == FD_STATE_SUCCESS) {
        if (fdnode->roomid != roomid) {
            //离开原有的房间
            leave_room(fdnode);

            //加入房间
            join_room(fdnode, roomid);
        }
        goto end;
    } else if (ev_type == EV_TYPE_GET_ROOM_LIST && fdnode->state == FD_STATE_SUCCESS) {
        send_roomlist(fdnode);
        goto end;
    } else if (ev_type == EV_TYPE_ADMIN_COMMAND_GOOUT && fdnode->state == FD_STATE_SUCCESS && fdnode->type == CLIENT_TYPE_ADMIN) {
        if (username) {
            clients *tonode;

            if (strcmp(fdnode->username, username) == 0) {
                tonode = fdnode;
            } else {
                tonode = get_fdnode_byname(username);
            }
            if (tonode != NULL) {
                bzero(msgbuffer, sizeof (msgbuffer));
                snprintf(msgbuffer, sizeof (msgbuffer), "<event type='%d' timestamp='%ld'><body><![CDATA[%s被管理员请出了聊天室]]></body></event>", EV_TYPE_MESSAGE, mytimestamp, tonode->username);
                send_message_all(tonode->fd, msgbuffer);
                node_del(tonode);
            }
        }
        goto end;
    } else if (ev_type == EV_TYPE_ADMIN_COMMAND_SHUTUP && fdnode->state == FD_STATE_SUCCESS && fdnode->type == CLIENT_TYPE_ADMIN) {
        if (username) {
            clients *tonode;

            if (strcmp(fdnode->username, username) == 0) {
                tonode = fdnode;
            } else {
                tonode = get_fdnode_byname(username);
            }
            if (tonode != NULL) {
                bzero(msgbuffer, sizeof (msgbuffer));
                if (tonode->type != CLIENT_TYPE_SHUTUP) {
                    tonode->type = CLIENT_TYPE_SHUTUP;
                    snprintf(msgbuffer, sizeof (msgbuffer), "<event type='%d' timestamp='%ld'><body><![CDATA[%s被管理员禁言]]></body></event>", EV_TYPE_MESSAGE, mytimestamp, tonode->username);
                } else {
                    tonode->type = CLIENT_TYPE_NONE;
                    snprintf(msgbuffer, sizeof (msgbuffer), "<event type='%d' timestamp='%ld'><body><![CDATA[%s被管理员解除禁言]]></body></event>", EV_TYPE_MESSAGE, mytimestamp, tonode->username);
                }
                send_message_all(tonode->fd, msgbuffer);
            }
        }
        goto end;
    }
    goto end;
end:
    //if (body!=NULL) free(body);
    ezxml_free(event);
}
