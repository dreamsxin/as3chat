/* 
 * File:   RecvMessage.cpp
 * Author: phpwind
 * 
 * Created on 2010年12月22日, 上午9:46
 */

#include "RecvMessage.h"
#include "ParseMessage.h"
#include "Log.h"

RecvMessage::RecvMessage(Client *client, Epoll &epoll, ThreadPool& pool, ClientList& clientList) {
    mClient = client;
    mEpoll = epoll;
    mPool = &pool;
    mClientList = &clientList;
}

RecvMessage::~RecvMessage() {
}

void RecvMessage::run() {
    if (mClient == NULL) {
        return;
    }
    try {
        string recv;
        if (!mClient->recv(recv)) {
            Log::debug("%s, %d, recv fail\n", __FILE__, __LINE__);
            mEpoll.Unregister(mClient->getSock());
            mClientList->removeClient(mClient);
        } else {
            mPool->ExecuteAsync(new ParseMessage(mClient, mEpoll, *mClientList));
        }
    } catch (...) {
        Log::debug("%s, %d, RecvMessage fail\n", __FILE__, __LINE__);
        mEpoll.Unregister(mClient->getSock());
        mClientList->removeClient(mClient);
    }
}


