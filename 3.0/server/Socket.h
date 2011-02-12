/* 
 * File:   Socket.h
 * Author: phpwind
 *
 * Created on 2010年12月17日, 下午12:49
 */

#ifndef SOCKET_H
#define	SOCKET_H
#include <string>
#include <iostream>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

typedef struct MsgHead{
        //int ev_type;
        int length;
} MsgHead;

//int MSG;  为消息的类型。在发送消息时初始化。
//int data_total_length; 为消息的总长 （不包括包头，可为0）为0的话 就没有数据只有包头。
//int data_package_length;  为每一次 发送或接受的 包的大小。与total_length 一起配合可以得到整个数据包的接收或者发送的过程。

class Socket {
public:
    Socket();
    Socket(const int fd);
    Socket(const Socket& sock);

    virtual Socket & operator =(const Socket & sock) {
        if (this == &sock)
            return *this;
        this->mSock = sock.mSock;
        this->mAddr = sock.mAddr;
        return *this;
    }
    virtual ~Socket();

    // Server initialization
    bool create();
    bool bind(const int port);
    bool listen() const;
    int accept() const;

    // Client initialization
    bool connect(const std::string host, const int port);
    void close();

    // Data Transimission
    int send(void *buf, size_t len) const;
    int recv(void *buf, size_t len) const;


    bool set_non_blocking(const bool);

    bool is_valid() const {
        return mSock != -1;
    }

    int getSock() const {
        return mSock;
    }

    enum {
        MAXHOSTNAME = 200,
        MAXCONNECTIONS = 5,
        MAXRECV = 500
    };

private:
    int mSock;
    sockaddr_in mAddr;
};

#endif	/* SOCKET_H */

