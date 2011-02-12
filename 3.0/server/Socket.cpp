/* 
 * File:   Socket.cpp
 * Author: phpwind
 * 
 * Created on 2010年12月17日, 下午12:49
 */

#include "Socket.h"

/*
const int Socket::MAXHOSTNAME = 200;
const int Socket::MAXCONNECTIONS = 5;
const int Socket::MAXRECV = 500;
 */
Socket::Socket() :
mSock(-1) {
    memset(&mAddr, 0, sizeof ( mAddr));

}

Socket::Socket(const int fd) :
mSock(fd) {
    memset(&mAddr, 0, sizeof ( mAddr));
}

Socket::Socket(const Socket & sock) {
    this->mSock = sock.mSock;
    this->mAddr = sock.mAddr;
}

Socket::~Socket() {
    close();
}

bool Socket::create() {
    mSock = socket(AF_INET, SOCK_STREAM, 0);

    if (!is_valid())
        return false;

    int on = 1;
    if (setsockopt(mSock, SOL_SOCKET, SO_REUSEADDR, (const char*) &on, sizeof ( on)) == -1)
        return false;

    return true;

}

bool Socket::bind(const int port) {

    if (!is_valid()) {
        return false;
    }

    mAddr.sin_family = AF_INET;
    mAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    mAddr.sin_port = htons(port);

    int bind_return = ::bind(mSock, (struct sockaddr *) &mAddr, sizeof ( mAddr));

    if (bind_return == -1) {
        return false;
    }

    return true;
}

bool Socket::listen() const {
    if (!is_valid()) {
        return false;
    }

    int listen_return = ::listen(mSock, MAXCONNECTIONS);


    if (listen_return == -1) {
        return false;
    }

    return true;
}

int Socket::accept() const {
    int addr_length = sizeof ( mAddr);
    return ::accept(mSock, (sockaddr *) & mAddr, (socklen_t *) & addr_length);
}

int Socket::send(void *buf, size_t len) const {
    return ::write(mSock, buf, len); // send , MSG_NOSIGNAL
}

int Socket::recv(void *buf, size_t len) const {    
    return ::read(mSock, buf, len); // recv, MSG_NOSIGNAL
}

bool Socket::connect(const std::string host, const int port) {
    if (!is_valid()) return false;

    mAddr.sin_family = AF_INET;
    mAddr.sin_port = htons(port);

    int status = inet_pton(AF_INET, host.c_str(), &mAddr.sin_addr);

    if (errno == EAFNOSUPPORT) return false;

    status = ::connect(mSock, (sockaddr *) & mAddr, sizeof ( mAddr));

    if (status == 0)
        return true;
    else
        return false;
}

void Socket::close() {
    if (is_valid())
        ::close(mSock);
}

bool Socket::set_non_blocking(const bool b) {
    int opts;

    opts = fcntl(mSock, F_GETFL);

    if (opts < 0) {
        return false;
    }

    if (b)
        opts = (opts | O_NONBLOCK);
    else
        opts = (opts & ~O_NONBLOCK);

    fcntl(mSock, F_SETFL, opts);
    if (fcntl(mSock, F_SETFL, opts) == -1) {
        return false;
    }
    return true;
}

