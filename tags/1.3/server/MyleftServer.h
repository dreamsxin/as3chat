/*
 * File:   MyleftServer.h
 * Author: Administrator
 *
 * Created on 2009年9月12日, 下午10:48
 */

#ifndef _MYLEFTSERVER_H
#define	_MYLEFTSERVER_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h> //相关函数atoi，atol，strtod，strtol，strtoul
#include <unistd.h>
#include <crypt.h>
#include <syslog.h>

#include <string.h>
#include <time.h>

#include <mysql/mysql.h>

#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <fcntl.h>
#include <pthread.h>
#include "hash.h"
#include "luafunction.h"
#include "ezxml/ezxml.h"
#include "util/util.h"

//data
#define DEBUG 1

#define RETURN_FAILURE -1
#define RETURN_SUCCESS 1

#define MAX_CLIENTS 1023
#define MAX_MAPS 24
#define MAX_CHAR_LENGTH 64
#define MAX_BUFFER_LENGTH 512

//消息类型
#define EV_TYPE_PING 0
#define EV_TYPE_AUTH 10
#define EV_TYPE_AUTH_OTHER_LOGIN 11
#define EV_TYPE_USER_LOGIN 20
#define EV_TYPE_USER_LOGOUT 21
#define EV_TYPE_USER_ADD 22
#define EV_TYPE_USER_CHANGE_STATE 23
#define EV_TYPE_MESSAGE 30
#define EV_TYPE_CHANGE_ROOM 40
#define EV_TYPE_GET_ROOM_LIST 41
#define EV_TYPE_ROOM_ADD 42
#define EV_TYPE_ADMIN_COMMAND_GOOUT 90
#define EV_TYPE_ADMIN_COMMAND_SHUTUP 91

//状态
#define FD_STATE_NONE 0
#define FD_STATE_WAIT 1
#define FD_STATE_SUCCESS 2

//类型
#define CLIENT_TYPE_NONE 0
#define CLIENT_TYPE_SHUTUP 1
#define CLIENT_TYPE_ADMIN 9

#define POLICY "<policy-file-request/>"
#define QUIT "<quit/>"
#define PING "<ping/>"
#define NONE "<none/>\n"

#define ALL_USERS "all"

#define T_MAX 50
#define MAX_FDS 20480
#define MAX_USERS 100
#define MAX_ROOMS 50
#define MAX_EVENTS 1024
    int t_min;
    int port;
    int epfd; //epoll句柄
    //声明epoll_event结构体的变量,ev用于注册事件,数组用于回传要处理的事件
    struct epoll_event ev, events[MAX_EVENTS];

    pthread_t tid[T_MAX];
    int t_num; //线程数
    pthread_mutex_t t_mutex;
    pthread_cond_t t_cond;
    //需要处理的任务列表

    struct task {
        struct clients *client;
        int logout;
        char *data;
        struct task *next;
    };
    struct task *task_head;

    //线程的任务函数
    void * readtask(void *args);

    //客户端连接数据结构体
    typedef struct clients {
        int fd;
        int roomid;
        int state;
        int type;
        int anonymous;
        char *username; //用户名
        struct clients *next;
    } clients;
    //客户端fd->fds关联数组
    clients *fd_clients[MAX_FDS];

    //聊天室房间
    typedef struct room {
        int enable;
        int num;
        char name[MAX_CHAR_LENGTH];
        struct clients *client;
    } room;

    room rooms[MAX_ROOMS];

    //用户名哈希


    typedef struct server {
        int listen_sockfd;
        struct sockaddr_in listen_addr;
    } server;

    //数据库
    MYSQL *conn;
    char dbhost[MAX_CHAR_LENGTH];
    int dbport;
    char dbuser[MAX_CHAR_LENGTH];
    char dbpass[MAX_CHAR_LENGTH];
    char dbname[MAX_CHAR_LENGTH];
    char dbtable[MAX_CHAR_LENGTH];
    char fieldusername[MAX_CHAR_LENGTH];
    char fieldpassword[MAX_CHAR_LENGTH];
    char fieldsalt[MAX_CHAR_LENGTH];
    char crossdomain[MAX_BUFFER_LENGTH];

    void db_connect();
    void db_close();

    MYSQL_RES *get_user_tuple(const char *username);
    int get_password(const char* username, char password[MAX_CHAR_LENGTH], char salt[MAX_CHAR_LENGTH]);

    FILE *fp;
    server s;

    //function
    void server_init();
    void on_accept(int fd, short ev, void *arg);
    void on_read(int fd, short ev, void *arg);

    void destory();

    //function
    char *strescape(const char *buf);
    void md5(const char *str, char *dec);
    int evutil_make_socket_nonblocking(int fd);

    int join_room(clients *p, int roomid);
    int leave_room(clients *p);
    int node_del(clients *p);

    void other_same_username(clients *node);
    void send_logout(clients *node);
    void send_userlist(clients *node);
    void send_roomlist(clients *node);
    void send_message(int fd, char* message);
    void send_message_all(int fd, char* message);


    int get_attribute(char* xml, char* attribute, char* buffer, int len);
    int get_tag(char* xml, char* tag, char* buffer, int len);

    clients *get_fdnode_byname(const char *uname);

    //message
    void *readtask(void *args);
    void parse_message(struct task * args);

    //log
    void log_write(int level, const char *msgfmt, char *file, int line, ...);
    void log_free();

#ifdef	__cplusplus
}
#endif

#endif	/* _MYLEFTSERVER_H */