/* 
 * File:   Epoll.cpp
 * Author: myleft
 * 
 * Created on 2010年12月16日, 下午11:16
 */

#include "Epoll.h"
#include <string.h>
Epoll::Epoll(unsigned int size)
: epfd(-1) {
    epfd = epoll_create(size);
    if (epfd == -1) {
        throw Exception("epoll_create() failed", errno);
    }
}

Epoll::~Epoll() {
    close(epfd);
}

void Epoll::Register(int fd, int events, const epoll_data_t *data) {
    Log::debug("%s, %d, Epoll Register\n", __FILE__, __LINE__);
    if (Ctrl(EPOLL_CTL_ADD, fd, events, data) == -1) {
        std::cerr << "Epoll::Register() failed" << errno << std::endl;
        throw Exception("Epoll::Register() failed", errno);
    }
}

//-----------------------------------------------------------------------------------------//

void Epoll::Update(int fd, int events, const epoll_data_t *data) {
    if (Ctrl(EPOLL_CTL_MOD, fd, events, data) == -1) {
        throw Exception("Epoll::Update() failed", errno);
    }
}

//-----------------------------------------------------------------------------------------//

void Epoll::Unregister(int fd) {
    Log::debug("%s, %d, Epoll Unregister\n", __FILE__, __LINE__);
    int result = epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
    if (result == -1 && errno != ENOENT) {
        throw Exception("Epoll::Unregister() failed", errno);
    }
}

//-----------------------------------------------------------------------------------------//

int Epoll::Select(epoll_event *events, size_t eventsSize, int timeout) {
    int result = epoll_wait(epfd, events, eventsSize, timeout);
    if (result == -1 && errno != EINTR) {
        throw Exception("Epoll::Select() failed", errno);
    }

    return result;
}

int Epoll::Ctrl(int op, int fd, int events, const epoll_data_t *data) {
    struct epoll_event ev;
    memset(&ev, 0, sizeof ( ev));
    ev.events = events;
    ev.data = *data;

    return epoll_ctl(epfd, op, fd, &ev);
}

