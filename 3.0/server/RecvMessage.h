/* 
 * File:   RecvMessage.h
 * Author: phpwind
 *
 * Created on 2010年12月22日, 上午9:46
 */

#ifndef RECVMESSAGE_H
#define	RECVMESSAGE_H
#include "ThreadTask.h"
#include "Client.h"
#include "ClientList.h"
#include "Epoll.h"
#include "ThreadPool.h"
#include "zlib/zlib.h"

class RecvMessage : public ThreadTask {
public:
    RecvMessage(Client *client, Epoll& epoll, ThreadPool& pool, ClientList& clientList);
    virtual ~RecvMessage();
    virtual void run();
private:
    Client * mClient;
    Epoll mEpoll;
    ThreadPool * mPool;
    ClientList * mClientList;
};

#endif	/* RECVMESSAGE_H */

