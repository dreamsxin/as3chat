/* 
 * File:   ClientList.h
 * Author: phpwind
 *
 * Created on 2010年12月20日, 下午3:45
 */

#ifndef CLIENTLIST_H
#define	CLIENTLIST_H
#include <map>
#include "Client.h"
using std::map;
using std::string;

class ClientList {
public:
    ClientList();
    virtual ~ClientList();
    
    bool addClient(Client* client);
    void removeClient(Client* client);
    Client* findClient(const string &name) const;
protected:
    map<string, Client *> mClients;
private:

};

#endif	/* CLIENTLIST_H */

