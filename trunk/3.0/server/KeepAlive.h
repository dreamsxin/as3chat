/* 
 * File:   KeepAlive.h
 * Author: phpwind
 *
 * Created on 2010年12月22日, 上午11:08
 */

#ifndef KEEPALIVE_H
#define	KEEPALIVE_H
#include <time.h> 
#include "ThreadTask.h"
#include "ClientList.h"
class KeepAlive : public ThreadTask {
public:
    KeepAlive(ClientList &);
    virtual ~KeepAlive();
    virtual void run();
private:
    ClientList *mClientList;
};

#endif	/* KEEPALIVE_H */

