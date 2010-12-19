/* 
 * File:   Epoll.h
 * Author: myleft
 *
 * Created on 2010年12月16日, 下午11:16
 */

#ifndef EPOLL_H
#define	EPOLL_H
#include <iostream>
#include <errno.h>
#include <sys/epoll.h>
#include "Exception.h"
using namespace std;

class Epoll {
public:
    explicit Epoll(unsigned int size = 1024);
    virtual ~Epoll();
    void Register(int fd, int events, const epoll_data_t *data);
    void Update(int fd, int events, const epoll_data_t *data);
    void Unregister(int fd);

    int Select(epoll_event *events, size_t eventsSize, int timeout);

    int GetFD() const {
        return epfd;
    };

private:
    int Ctrl(int op, int fd, int events, const epoll_data_t *data);

    int epfd;

};

#endif	/* EPOLL_H */

