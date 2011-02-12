/* 
 * File:   Client.h
 * Author: phpwind
 *
 * Created on 2010年12月20日, 下午2:03
 */

#ifndef CLIENT_H
#define	CLIENT_H
#include <string>
#include <time.h>
#include "Socket.h"
#include "BuddyList.h"
#include "FifoList.h"

class Client : public Socket {
public:
    Client(int fd, int maxSize = 10);
    virtual ~Client();

    bool send(std::string&) const;
    bool recv(std::string&);

    string getName() const {
        return mName;
    };
    void addBuddy(const std::string& group, const std::string& buddy);
    void removeBuddy(const std::string& buddy);
    bool isBuddy(const std::string& buddy) const;
    list<string> getBuddies() const;

    void pushInPacket(const std::string& packer);
    string getNextInPacket();

    void pushOutPacket(const std::string& packer);
    string getNextOutPacket();

    int lastresponsetime;
private:
    string mName;
    BuddyList *mBuddyList;
    FifoList<string> *mInPackets;
    FifoList<string> *mOutPackets;
};

#endif	/* CLIENT_H */

