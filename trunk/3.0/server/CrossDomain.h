/* 
 * File:   CrossDomain.h
 * Author: phpwind
 *
 * Created on 2010年12月22日, 上午10:05
 */

#ifndef CROSSDOMAIN_H
#define	CROSSDOMAIN_H
#include "ThreadTask.h"
#include "Socket.h"
#include "Epoll.h"

class CrossDomain : public ThreadTask {
public:
    CrossDomain();
    virtual ~CrossDomain();
    virtual void run();
private:
};

#endif	/* CROSSDOMAIN_H */

