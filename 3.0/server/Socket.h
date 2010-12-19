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

const int MAXHOSTNAME = 200;
const int MAXCONNECTIONS = 5;
const int MAXRECV = 500;

class Socket {
public:
    Socket();
    Socket(const int fd);
    Socket(const Socket& orig);

    virtual Socket & operator =(const Socket & sock) {
        if (this == &sock)
            return *this;
        this->m_sock = sock.m_sock;
        this->m_addr = sock.m_addr;
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
    bool send(const std::string) const;
    int recv(std::string&) const;


    bool set_non_blocking(const bool);

    bool is_valid() const {
        return m_sock != -1;
    }

    int get_sockfd() const {
        return m_sock;
    }

private:
    int m_sock;
    sockaddr_in m_addr;

};

#endif	/* SOCKET_H */

