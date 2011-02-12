/* 
 * File:   parseMessage.h
 * Author: phpwind
 *
 * Created on 2010年12月21日, 下午1:06
 */

#ifndef PARSEMESSAGE_H
#define	PARSEMESSAGE_H
#include "ThreadTask.h"
#include "Client.h"
#include "ClientList.h"
#include "Epoll.h"
#include "MessageType.h"
#include "Log.h"
#include "tinyxml/tinyxml.h"

class ParseMessage : public ThreadTask {
public:
    ParseMessage(Client *client, Epoll& epoll, ClientList& clientList);
    virtual ~ParseMessage();
    virtual void run();
private:
    Client * mClient;
    Epoll mEpoll;
    ClientList * mClientList;
};

#endif	/* PARSEMESSAGE_H */

