/*
 * File:   MyleftServer.c
 * Author: Administrator
 *
 * Created on 2009年9月12日, 下午10:48
 */

/**
 * int getopt(int argc, char * const argv[], const char *optstring);
 * extern char *optarg;
 * extern int optind, opterr, optopt;
 **/

#include "MyleftServer.h"

/*
 *
 */
int main() {
    t_min = 5;

    port = 5222;

    strncpy(dbhost, "localhost", MAX_CHAR_LENGTH);
    dbport = 3306;
    strncpy(dbuser, "root", MAX_CHAR_LENGTH);
    strncpy(dbpass, "123456", MAX_CHAR_LENGTH);
    strncpy(dbname, "ucenter", MAX_CHAR_LENGTH);
    strncpy(dbtable, "uc_members", MAX_CHAR_LENGTH);
    strncpy(fieldusername, "username", MAX_CHAR_LENGTH);
    strncpy(fieldpassword, "password", MAX_CHAR_LENGTH);

    int i;
    for (i = 0; i < MAX_FDS; i++) {
        fd_clients[i] = NULL;
    }

    for (i = 0; i < MAX_ROOMS; i++) {
        rooms[i].enable = 0;
        rooms[i].num = 0;
        memset(rooms[i].name, '\0', sizeof (rooms[i].name));
        rooms[i].client = NULL;
    }

    if (hash_create(MAX_USERS) == 0) {
        log_write(LOG_ERR, "hcreate error", __FILE__, __LINE__);
        return (EXIT_FAILURE);
    }

    rooms[0].enable = 1;
    strncpy(rooms[0].name, "Myleft聊天大厅", MAX_CHAR_LENGTH);

    lua_init();
    lua_load_config();
    lua_load_room();

    db_connect();

    //创建epoll句柄
    epfd = epoll_create(MAX_FDS);

    int optval = 1; // 关闭之后重用socket
    unsigned int optlen = sizeof (optval);

    if ((s.listen_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        log_write(LOG_ERR, "socket error", __FILE__, __LINE__);
        destory();
        return (EXIT_FAILURE);
    }

    log_write(LOG_DEBUG, "listen socked created", __FILE__, __LINE__);

    setsockopt(s.listen_sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof (optlen)); //端口重用，如意外没有释放端口，还可以绑定成功

    bzero(&s.listen_addr, sizeof (s.listen_addr)); //memset 适合大数据

    s.listen_addr.sin_family = AF_INET;
    s.listen_addr.sin_port = htons(port);
    s.listen_addr.sin_addr.s_addr = htonl(INADDR_ANY); //inet_addr("127.0.0.1");

    if (bind(s.listen_sockfd, (struct sockaddr*) & s.listen_addr, sizeof (s.listen_addr)) < 0) {
        log_write(LOG_ERR, "bind error", __FILE__, __LINE__);
        destory();
        return (EXIT_FAILURE);
    }

    if (listen(s.listen_sockfd, 3) < 0) {
        log_write(LOG_ERR, "listen error", __FILE__, __LINE__);
        destory();
        return (EXIT_FAILURE);
    }
    printf("listenning......\n");

    /* 设置socket为非阻塞模式，使用libevent编程这是必不可少的。 */
    if (evutil_make_socket_nonblocking(s.listen_sockfd) < 0) {
        log_write(LOG_ERR, "evutil_make_socket_nonblocking error", __FILE__, __LINE__);
        destory();
        return (EXIT_FAILURE);
    }

    //设置与要处理的事件相关的文件描述符
    ev.data.fd = s.listen_sockfd;
    //设置要处理的事件类型
    ev.events = EPOLLIN;

    int nfds;
    int client_fd;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof (client_addr);

    char msgbuffer[MAX_BUFFER_LENGTH];
    int recv_bits;

    //注册epoll事件
    epoll_ctl(epfd, EPOLL_CTL_ADD, s.listen_sockfd, &ev);

    pthread_mutex_init(&t_mutex, NULL);
    pthread_mutex_init(&hashlock, NULL);
    pthread_cond_init(&t_cond, NULL);

    //开启心跳线程heartbeat
    pthread_create(&t_heartbeat, NULL, heartbeat, NULL);

    //初始化任务线程，开启两个线程来完成任务，线程之间会互斥地访问任务链表
    t_num = 0;
    while (t_num < t_min && t_num < T_MAX) {
        t_num++;
        pthread_create(&tid[t_num], NULL, readtask, NULL);
    }

    clients *client_fdnode;
    int logout = 0;
    while (1) {
        //等待epoll事件的发生
        nfds = epoll_wait(epfd, events, MAX_EVENTS, 3000);
        //处理所发生的所有事件
        for (i = 0; i < nfds; ++i) {
            time(&mytimestamp);
            p = gmtime(&mytimestamp);
            printf("fd:%d, file:%s, line:%d\n", events[i].data.fd, __FILE__, __LINE__);
            if (events[i].data.fd == s.listen_sockfd) {//主socket
                client_fd = accept(s.listen_sockfd, (struct sockaddr *) & client_addr, &client_len);
                if (client_fd < 0) {
                    perror("connfd<0");
                }

                evutil_make_socket_nonblocking(client_fd);

                clients *node = (clients *) malloc(sizeof (clients));
                node->fd = client_fd;
                node->roomid = 0;
                node->state = FD_STATE_WAIT;
                node->type = CLIENT_TYPE_NONE;
                node->anonymous = 1;
                node->x = 1;
                node->y = 1;
                node->keepalivetime = mytimestamp;
                node->username = (char *) malloc(MAX_CHAR_LENGTH * sizeof (char));
                node->next = NULL;
                fd_clients[client_fd] = node;


                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = client_fd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &ev);
                log_write(LOG_DEBUG, "epoll_ctl", __FILE__, __LINE__);
            } else if (events[i].events & EPOLLIN) {
                logout = 0;
                memset(msgbuffer, '\0', sizeof (msgbuffer));
                printf("msgbuffer:%s", msgbuffer);
                if (events[i].data.fd < 0) {
                    continue;
                }
                client_fdnode = fd_clients[events[i].data.fd];
                if (client_fdnode==NULL) {
                    events[i].data.fd = -1;
                    close(events[i].data.fd);
                }
                client_fdnode->keepalivetime = mytimestamp;
                recv_bits = read(events[i].data.fd, msgbuffer, sizeof (msgbuffer));
                if (recv_bits <= 0) {
                    printf("recv_bits:%d, file:%s, line:%d\n", recv_bits, __FILE__, __LINE__);
                    logout = 1;

                    events[i].data.fd = -1;
                    if (client_fdnode->state != FD_STATE_SUCCESS) {
                        node_del(client_fdnode);
                        continue; //退出
                    }
                } else if (recv_bits >= MAX_BUFFER_LENGTH) {
                    log_write(LOG_DEBUG, "recv_bits>", __FILE__, __LINE__);
                    logout = 1;

                    events[i].data.fd = -1;
                    if (client_fdnode->state != FD_STATE_SUCCESS) {
                        node_del(client_fdnode);
                        continue; //退出
                    }
                } else if (strncmp(msgbuffer, PING, sizeof (PING)) == 0) {
                    client_fdnode->keepalivetime = mytimestamp;
                    log_write(LOG_DEBUG, "PING", __FILE__, __LINE__);
                    send_message(client_fdnode->fd, "<event type='0'/>");
                    continue; //退出
                } else if (strncmp(msgbuffer, QUIT, sizeof (QUIT)) == 0) {
                    log_write(LOG_DEBUG, "QUIT", __FILE__, __LINE__);
                    logout = 1;

                    events[i].data.fd = -1;
                    if (client_fdnode->state != FD_STATE_SUCCESS) {
                        node_del(client_fdnode);
                        continue; //退出
                    }
                } else if (strncmp(msgbuffer, POLICY, sizeof (POLICY)) == 0) {
                    log_write(LOG_DEBUG, "send message crossdomain", __FILE__, __LINE__);
                    //send_message(events[i].data.fd, crossdomain);
                    write(events[i].data.fd, crossdomain, MAX_BUFFER_LENGTH);
                    continue; //退出
                } else if (strncmp(msgbuffer, PING, sizeof (PING)) == 0) {
                    log_write(LOG_DEBUG, "PING", __FILE__, __LINE__);
                    continue; //退出
                }
                log_write(LOG_DEBUG, "添加新的读任务", __FILE__, __LINE__);
                //添加新的读任务
                struct task *new_task = (struct task *) malloc(sizeof (struct task));
                if (new_task != NULL) {
                    new_task->client = client_fdnode;
                    new_task->logout = logout;
                    new_task->data = (char *) malloc(MAX_BUFFER_LENGTH * sizeof (char));
                    strcpy(new_task->data, msgbuffer);
                    new_task->next = NULL;
                    log_write(LOG_DEBUG, "new_task", __FILE__, __LINE__);
                    pthread_mutex_lock(&t_mutex);

                    if (task_head == NULL) {
                        task_head = new_task;
                    } else {
                        struct task *temp_task;
                        temp_task = task_head;
                        while (temp_task->next != NULL) {
                            temp_task = temp_task->next;
                        }
                        temp_task->next = new_task;
                    }
					//唤醒其中一个线程即可
					log_write(LOG_DEBUG, "唤醒所有等待cond1条件的线程", __FILE__, __LINE__);
					pthread_cond_signal(&t_cond);

					//唤醒所有等待cond1条件的线程
                    //log_write(LOG_DEBUG, "唤醒所有等待cond1条件的线程", __FILE__, __LINE__);
                    //pthread_cond_broadcast(&t_cond);

                    pthread_mutex_unlock(&t_mutex);
                }
            } else if (events[i].events & EPOLLOUT) {
                if (events[i].data.fd < 0) {
                    continue;
                }

                //设置用于读操作的文件描述符
                ev.data.fd = events[i].data.fd;
                //设置用于注测的读操作事件
                ev.events = EPOLLIN | EPOLLET;
                //修改sockfd上要处理的事件为EPOLIN
                epoll_ctl(epfd, EPOLL_CTL_MOD, events[i].data.fd, &ev);
            } else if (events[i].events & EPOLLRDHUP) {//tcp连接中断检测
                if (events[i].data.fd < 0) {
                    continue;
                }

                client_fdnode = fd_clients[events[i].data.fd];
                if (client_fdnode==NULL) {
                    events[i].data.fd = -1;
                    close(events[i].data.fd);
                }
                log_write(LOG_DEBUG, "EPOLLRDHUP 对方断开", __FILE__, __LINE__);

                events[i].data.fd = -1;
                node_del(client_fdnode);
                continue;

            } else if (events[i].events & EPOLLERR) {//tcp连接中断检测
                if (events[i].data.fd < 0) {
                    continue;
                }

                client_fdnode = fd_clients[events[i].data.fd];
                if (client_fdnode==NULL) {
                    events[i].data.fd = -1;
                    close(events[i].data.fd);
                }
                log_write(LOG_DEBUG, "EPOLLERR 对方断开", __FILE__, __LINE__);

                events[i].data.fd = -1;
                node_del(client_fdnode);
                continue;

            } else {
                perror("other event");
            }
            printf("一次小循环\n");
        }
    }

    close(s.listen_sockfd);
    destory();
    return (EXIT_SUCCESS);
}

//注销程序

void destory() {
    pthread_cancel(&t_heartbeat);
    for (t_num; t_num>0; t_num--) {
        pthread_cancel(&tid[t_num]);
    }
    
    pthread_mutex_destroy(&t_mutex);
    pthread_mutex_destroy(&hashlock);
    pthread_cond_destroy(&t_cond);
    db_close();
    hash_destroy();
    if (fp) fclose(fp);
    int i;
    for (i = 0; i < MAX_FDS; i++) {
        if (fd_clients[i] != NULL) {
            node_del(fd_clients[i]);
        }
    }
}