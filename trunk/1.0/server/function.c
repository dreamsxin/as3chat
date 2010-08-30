#include "MyleftServer.h"
void sleep_thread(int sec);

void *heartbeat(void *args) {
    int i;
    while (1) {
        sleep_thread(20);
        time(&mytimestamp);
        p = gmtime(&mytimestamp);
        for (i = 0; i < MAX_FDS; i++) {
            if (fd_clients[i] == NULL) {
                continue;
            }
            if ((mytimestamp - fd_clients[i]->keepalivetime) > 60) {
                log_write(LOG_DEBUG, "60秒内没有活动自动退出！", __FILE__, __LINE__);
                node_del(fd_clients[i]);
            }
        }
        pthread_testcancel();
    }
    pthread_exit(NULL);
}

void sleep_thread(int sec) {
    struct timespec wake;

    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

    time(&mytimestamp);
    p = gmtime(&mytimestamp);

    wake.tv_sec = mytimestamp + sec;

    //如果把上面的sec变成msec，并替换成下面的两句，就是实现微秒级别睡眠
    //nsec = now.tv_usec * 1000 + (msec % 1000000) * 1000;
    //wake.tv_sec = now.tv_sec + msec / 1000000 + nsec / 1000000000;
    wake.tv_nsec = 0;

    pthread_mutex_lock(&mutex);
    pthread_cond_timedwait(&cond, &mutex, &wake);
    pthread_mutex_unlock(&mutex);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
}

void md5(const char *str, char *dec) {
    MD5_CTX ctx;
    unsigned char digest[16];

    MD5_Init(&ctx);
    MD5_Update(&ctx, str, strlen(str));
    MD5_Final(digest, &ctx);
    char tmp[3] = {'\0'};
    int i;
    for (i = 0; i < sizeof (digest); i++) {
        sprintf(tmp, "%02x", digest[i]);
        strcat(dec, tmp);
    }
    printf("%s\n", dec);
}

int evutil_make_socket_nonblocking(int fd) {

    if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1) {
        log_write(LOG_ERR, "fcntl(O_NONBLOCK) failed", __FILE__, __LINE__);
        return (EXIT_FAILURE);
    }
    return (EXIT_SUCCESS);
}

int join_room(clients *p, int roomid) {

    char msgbuffer[MAX_BUFFER_LENGTH];
    printf("join_room:%d\n", roomid);

    if (p == NULL || roomid > MAX_ROOMS || roomid < 0 || rooms[roomid].enable == 0 || p->roomid == roomid) {
        return RETURN_FAILURE;
    }
    if (p->roomid >= 0) {
        leave_room(p);
    }
    pthread_mutex_lock(&t_mutex_room[roomid]);
    if (roomid < MAX_ROOMS && roomid>-1 && rooms[roomid].enable) {
        p->roomid = roomid;

        if (rooms[roomid].client == NULL) {
            rooms[roomid].client = p;
        } else {
            clients *client = rooms[roomid].client;
            log_write(LOG_DEBUG, "第二位之后用户", __FILE__, __LINE__);
            while (client->next != NULL) {
                log_write(LOG_DEBUG, "下移一位", __FILE__, __LINE__);
                client = client->next;
            }
            client->next = p;
            p->next = NULL;
        }
        rooms[roomid].num++;
        //通知自己已经进入房间
        printf("通知自己已经进入房间:%d\n", p->fd, __FILE__, __LINE__);
        bzero(msgbuffer, sizeof (msgbuffer));
        snprintf(msgbuffer, sizeof (msgbuffer), "<event type='%d' roomid='%d' name='%s' message='成功进入房间'/>", EV_TYPE_CHANGE_ROOM, roomid, rooms[roomid].name);
        send_message(p->fd, msgbuffer);

        //通知所有用户有人进入房间
        bzero(msgbuffer, sizeof (msgbuffer));
        snprintf(msgbuffer, sizeof (msgbuffer), "<event type='%d' username='%s'/>", EV_TYPE_USER_LOGIN, p->username);
        send_message_all(p->fd, msgbuffer);

        //发送用户列表
        log_write(LOG_DEBUG, "发送用户列表", __FILE__, __LINE__);
        send_userlist(p);
        log_write(LOG_ERR, "roomresetlock", __FILE__, __LINE__);
    }
    pthread_mutex_unlock(&t_mutex_room[roomid]);
    return RETURN_SUCCESS;
}

int leave_room(clients *p) {

    char msgbuffer[MAX_BUFFER_LENGTH];
    log_write(LOG_ERR, "leave_room", __FILE__, __LINE__);

    if (p == NULL || p->roomid > MAX_ROOMS || p->roomid < 0) {
        return RETURN_FAILURE;
    }
    int roomid = p->roomid;
    pthread_mutex_lock(&t_mutex_room[roomid]);
    if (roomid < MAX_ROOMS && roomid>-1 && rooms[p->roomid].client != NULL) {
        log_write(LOG_ERR, "leave_room2", __FILE__, __LINE__);
        clients *client = rooms[roomid].client;
        clients *prev = NULL;
        log_write(LOG_ERR, "leave_room-", __FILE__, __LINE__);
        while (client != NULL) {
            log_write(LOG_ERR, "leave_room--", __FILE__, __LINE__);
            if (client->fd == p->fd) {
                rooms[roomid].num--;
                log_write(LOG_ERR, "leave_room3", __FILE__, __LINE__);
                if (prev != NULL) {
                    prev->next = client->next;
                } else {
                    rooms[roomid].client = client->next;
                }
                break;
            }
            prev = client;
            client = client->next;
        }

        if (p->state == FD_STATE_SUCCESS) {
            bzero(msgbuffer, sizeof (msgbuffer));
            snprintf(msgbuffer, sizeof (msgbuffer), "<event type='%d' username='%s'/>", EV_TYPE_USER_LOGOUT, p->username);
            send_message_all(p->fd, msgbuffer);
            p->roomid = -1;
        }
        log_write(LOG_ERR, "roomresetlock", __FILE__, __LINE__);
    }
    pthread_mutex_unlock(&t_mutex_room[roomid]);
    log_write(LOG_DEBUG, "leave_room end", __FILE__, __LINE__);
    return RETURN_SUCCESS;
}

int node_add(clients *p) {
    log_write(LOG_ERR, "node_add", __FILE__, __LINE__);
    if (p != NULL) {
        log_write(LOG_ERR, "node_add2", __FILE__, __LINE__);
        hash_item *item = (hash_item *) malloc(sizeof (hash_item));
        item->key = p->username;
        item->data = (void *) p->fd;
        item->next = NULL;
        hash_add(item);
    }
    log_write(LOG_ERR, "node_add end", __FILE__, __LINE__);
    return RETURN_SUCCESS;
}

int node_del(clients *p) {
    log_write(LOG_ERR, "node_del", __FILE__, __LINE__);
    if (p != NULL) {
        close(p->fd);
        log_write(LOG_ERR, "node_del--", __FILE__, __LINE__);
        leave_room(p);
        hash_del(p->username, p->fd);
        fd_clients[p->fd] = NULL;
        free(p);
        p = NULL;
        log_write(LOG_DEBUG, "node_del11", __FILE__, __LINE__);
    }
    log_write(LOG_ERR, "node_del2", __FILE__, __LINE__);
    return RETURN_SUCCESS;
}

void send_userlist(clients *node) {
    printf("roomid:%d,enable:%d, name:%s\n", node->roomid, rooms[node->roomid].enable, rooms[node->roomid].name);
    if (node->roomid < MAX_ROOMS && node->roomid>-1 && rooms[node->roomid].enable) {
        clients *p = rooms[node->roomid].client;
        char msgbuffer[MAX_BUFFER_LENGTH];
        while (p != NULL) {
            if (p->fd != node->fd && p->state == FD_STATE_SUCCESS) {
                bzero(msgbuffer, sizeof (msgbuffer));
                snprintf(msgbuffer, sizeof (msgbuffer), "<event type='%d' username='%s' x='%d' y='%d' />", EV_TYPE_USER_ADD, p->username, p->x, p->y);
                send_message(node->fd, msgbuffer);
            }
            p = p->next;
        }
    }
}

void send_roomlist(clients *node) {
    char msgbuffer[MAX_BUFFER_LENGTH];
    int i;
    for (i = 0; i < MAX_ROOMS; i++) {
        if (rooms[i].enable) {
            printf("第%d房间:%s\n", i, rooms[i].name);
            bzero(msgbuffer, sizeof (msgbuffer));
            snprintf(msgbuffer, sizeof (msgbuffer), "<event type='%d' roomid='%d' name='%s' num='%d'/>", EV_TYPE_ROOM_ADD, i, rooms[i].name, rooms[i].num);
            send_message(node->fd, msgbuffer);
        } else {
            break;
        }
    }
}

void other_same_username(clients *node) {
    char msgbuffer[MAX_BUFFER_LENGTH];
    if (node != NULL) {
        leave_room(node);
        log_write(LOG_DEBUG, "同名用户存在", __FILE__, __LINE__);
        if (node->state == FD_STATE_SUCCESS) {
            node->state = FD_STATE_WAIT;

            bzero(msgbuffer, sizeof (msgbuffer));
            snprintf(msgbuffer, sizeof (msgbuffer), "<event type='%d' message='用户在其他地方登陆'/>", EV_TYPE_AUTH_OTHER_LOGIN);
            send_message(node->fd, msgbuffer);

            bzero(msgbuffer, sizeof (msgbuffer));

            //通知所有用户他退出了
            snprintf(msgbuffer, sizeof (msgbuffer), "<event type='%d' username='%s'/>", EV_TYPE_USER_LOGOUT, node->username);
            send_message_all(node->fd, msgbuffer);
        }

    }
}

clients *get_fdnode_byname(const char *uname, int ufd) {
    log_write(LOG_ERR, "get_fdnode_byname", __FILE__, __LINE__);
    log_write(LOG_ERR, uname, __FILE__, __LINE__);
    hash_item *found_item = hash_search(uname);
    int fd = -1;
    while (found_item != NULL) {
        log_write(LOG_ERR, found_item->key, __FILE__, __LINE__);
        if (strcmp(found_item->key, uname) == 0 && (ufd < 0 || (int) found_item->data != ufd)) {
            fd = (int) found_item->data;
            break;
        }
        found_item = found_item->next;
    }

    return fd >= 0 ? fd_clients[fd] : NULL;
}

void send_message(const int fd, const char* message) {
    printf("message:%s, to :%d,file:%s, line:%d\n", message, fd, __FILE__, __LINE__);
    int ret = -1;
    if (message != NULL) {
       ret = write(fd, message, strlen(message));
    }
}
//包括自己

void send_message_all(const int fd, const char* message) {

    if (fd_clients[fd] == NULL || message == NULL) return;
    clients *client = fd_clients[fd];
    if (client->roomid < MAX_ROOMS && client->roomid>-1 && rooms[client->roomid].client != NULL) {
        printf("send_message_all:%s, file:%s, line:%d\n", message, __FILE__, __LINE__);
        clients *p = rooms[client->roomid].client;
        while (p != NULL) {
            if (p->state == FD_STATE_SUCCESS) {
                printf("message:%s, to :%d,file:%s, line:%d\n", message, p->fd, __FILE__, __LINE__);
                send_message(p->fd, message);
            }
            p = p->next;
        }
    }
}

int get_attribute(char* xml, char* attribute, char* buffer, int len) {

    // Get to the start of the attribute
    char* as = strstr(xml, attribute);
    if (!as) return (RETURN_FAILURE);

    // Read the attribute
    char* start = NULL;
    int numchars = 0;
    int i = 0;
    for (i = 0; i < strlen(as); i++) {

        if (start) numchars++;

        if (*(as + i) == '\'' || *(as + i) == '"') {
            if (!start)
                start = (as + i + 1);
            else
                break;
        }
    }

    // Store the result in the buffer
    if (numchars > len) return (RETURN_FAILURE);
    strncpy(buffer, start, numchars);
    *(buffer + numchars - 1) = '\0';
    return (RETURN_SUCCESS);
}

// Extracts the contents of the xml tag given and stores it in the buffer

int get_tag(char* xml, char* tag, char* buffer, int len) {

    char tagopen[32];
    char tagclose[32];
    char inone[32];
    snprintf(tagopen, sizeof (tagopen), "<%s", tag);
    snprintf(tagclose, sizeof (tagclose), "</%s>", tag);
    snprintf(inone, sizeof (inone), "<%s/>", tag);

    // If the tag is present, but has no content, stop now
    if (strstr(xml, inone)) return (RETURN_SUCCESS);
    // Get to the start of the tag
    char* as = strstr(xml, tagopen);
    if (!as) return (RETURN_FAILURE);
    char* ap = strstr(as, ">") + sizeof (char);
    if (!as) return (RETURN_FAILURE);
    // Find the end

    int i = 0;
    while (*(ap + (sizeof (char) * i)) != '<') i++;
    // Store the result in the buffer

    if (i > len) return (RETURN_FAILURE);
    strncpy(buffer, ap, i);

    *(buffer + i) = '\0';
    return (RETURN_SUCCESS);
}