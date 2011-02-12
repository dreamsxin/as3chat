/* 
 * File:   Client.cpp
 * Author: phpwind
 * 
 * Created on 2010年12月20日, 下午2:03
 */

#include "Client.h"
#include "Log.h"

Client::Client(int fd, int maxSize) : Socket(fd) {
    mBuddyList = new BuddyList;
    mInPackets = new FifoList<string > (maxSize);
    mOutPackets = new FifoList<string > (maxSize);
}

Client::~Client() {
    Log::debug("%s, %d, Client::~Client\n", __FILE__, __LINE__);
    delete mBuddyList;
    delete mInPackets;
    delete mOutPackets;
}

// 覆盖父类

bool Client::send(std::string& msg) const {
    Log::debug("%s, %d, Client::send\n", __FILE__, __LINE__);
    MsgHead msghead;
    msghead.length = htonl(msg.size());
    int send = Socket::send(&msghead, sizeof(msghead));
    if (send == sizeof(MsgHead)) {

        Log::debug("%s, %d, Client::send MsgHead:%d\n", __FILE__, __LINE__, msghead.length);
        const char *p = msg.c_str();
        char* temp = new char[msg.size() + 1];
        strcpy(temp, p);
        send = Socket::send(temp, msg.size());
        if (send > 0) {
            Log::debug("%s, %d, Client::send msg:%s\n", __FILE__, __LINE__, temp);
            return true;
        }
    }
    
    return false;
}

bool Client::recv(std::string& msg) {
    msg = "";

    char buf [ MAXRECV + 1 ];

    MsgHead msghead;

    int recv = Socket::recv(&msghead, sizeof(msghead));
    if (recv != sizeof(MsgHead)) {
        return false;
    }
    lastresponsetime = ::time(NULL);

    int notreadlen = msghead.length = ntohl(msghead.length);
    
    Log::debug("%s, %d, Client::recv, len:%d\n", __FILE__, __LINE__, msghead.length);
    memset(buf, '\0', sizeof (buf));
    unsigned long toreadlen = notreadlen > MAXRECV ? MAXRECV : notreadlen;
    while (toreadlen > 0 && (recv = Socket::recv(buf, toreadlen)) > 0) {
        msg.append(buf);
        memset(buf, '\0', sizeof (buf));
        notreadlen = notreadlen - recv;
        toreadlen = notreadlen > MAXRECV ? MAXRECV : notreadlen;
    }
    Log::debug("%s, %d, Client::recv, msg:%s\n", __FILE__, __LINE__, msg.c_str());
    pushInPacket(msg);
    if (toreadlen == 0) {
        return true;
    }

    Log::debug("%s, %d, Client::recv, notreadlen:%d\n", __FILE__, __LINE__, notreadlen);
 
    return false;
}

// 好友

void Client::addBuddy(const std::string& group, const std::string& buddy) {
    if (mBuddyList != NULL) mBuddyList->addBuddy(group, buddy);
}

void Client::removeBuddy(const std::string& buddy) {
    if (mBuddyList != NULL) mBuddyList->removeBuddy(buddy);
}

bool Client::isBuddy(const std::string& buddy) const {
    if (mBuddyList != NULL) return mBuddyList->isBuddy(buddy);
    return false;
}

std::list<std::string> Client::getBuddies() const {
    if (mBuddyList != NULL) return mBuddyList->getBuddies(mName);
    std::list<string> buddies;
    return buddies;
}

// 接收到的网络包

void Client::pushInPacket(const std::string& packet) {
    mInPackets->push(packet);
}

std::string Client::getNextInPacket() {
    try {
        return mInPackets->pop();
    } catch (std::out_of_range&) {
        return "";
    }

}

// 发送的网络包

void Client::pushOutPacket(const std::string& packet) {
    mOutPackets->push(packet);
}

std::string Client::getNextOutPacket() {
    try {
        return mOutPackets->pop();
    } catch (std::out_of_range&) {
        return "";
    }

}
