/* 
 * File:   ClientList.cpp
 * Author: phpwind
 * 
 * Created on 2010年12月20日, 下午3:45
 */

#include "ClientList.h"

ClientList::ClientList() {
}

ClientList::~ClientList() {
}

bool ClientList::empty() {
    return mNameClients.empty();

}

bool ClientList::addClient(Client* client) {
    std::lock_guard<std::mutex> lock(mMutex);
    if (!client->getName().empty()) {
        std::pair < map<string, Client*>::iterator, bool> insertPair;
        insertPair = mNameClients.insert(std::make_pair(client->getName(), client));
        if (!insertPair.second) {
            return false;
        }
    }
    std::pair < map<int, Client*>::iterator, bool> insertPair2;
    insertPair2 = mClients.insert(std::make_pair(client->getSock(), client));
    if (!insertPair2.second) {
        removeClient(client);
        return false;
    }
    return true;
};

void ClientList::removeClient(Client* client) {
    std::lock_guard<std::mutex> lock(mMutex);
    Log::debug("%s, %d, ClientList::removeClient\n", __FILE__, __LINE__);
    map<string, Client*>::iterator iter;
    iter = mNameClients.find(client->getName());
    if (iter != mNameClients.end()) {
        mNameClients.erase(iter);
    }

    map<int, Client*>::iterator iter2;
    iter2 = mClients.find(client->getSock());
    if (iter2 != mClients.end()) {
        mClients.erase(iter2);
    }
    delete client;
    client = NULL;
};

Client* ClientList::findClient(const string &name) const {
    map<string, Client*>::const_iterator iter;
    iter = mNameClients.find(name);
    if (iter != mNameClients.end()) {
        return iter->second;
    }
    return NULL;
};

Client* ClientList::findClient(const int fd) const {
    map<int, Client*>::const_iterator iter;
    iter = mClients.find(fd);
    if (iter != mClients.end()) {
        return iter->second;
    }
    return NULL;
};

void ClientList::keepAlive() {;
    std::map<int, Client *>::iterator iter;
    for (iter = mClients.begin(); iter != mClients.end();) {
        if (iter->second->lastresponsetime - ::time(NULL) >= 60)
            mClients.erase(iter++);
        else
            ++iter;
    }
}