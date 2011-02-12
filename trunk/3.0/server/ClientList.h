/* 
 * File:   ClientList.h
 * Author: phpwind
 *
 * Created on 2010年12月20日, 下午3:45
 */

#ifndef CLIENTLIST_H
#define	CLIENTLIST_H
#include <map>
#include <mutex>
#include "Client.h"
#include "Log.h"
using std::map;
using std::string;

class ClientList {
public:
    ClientList();
    virtual ~ClientList();
    bool empty();
    bool addClient(Client* client);
    void removeClient(Client* client);
    Client* findClient(const string &name) const;
    Client* findClient(const int fd) const;

    void keepAlive();
protected:
    map<string, Client *> mNameClients;
    map<int, Client *> mClients;
private:
    std::mutex mMutex;
};

#endif	/* CLIENTLIST_H */

