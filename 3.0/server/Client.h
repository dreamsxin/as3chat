/* 
 * File:   Client.h
 * Author: phpwind
 *
 * Created on 2010年12月20日, 下午2:03
 */

#ifndef CLIENT_H
#define	CLIENT_H
#include <string>
#include "Socket.h"
#include "BuddyList.h"
#include "FifoList.h"

class Client : public Socket {
public:
    Client(int fd, int maxSize = 10);
    Client(int fd, BuddyList &buddyList, int maxSize = 10);
    virtual ~Client();

    int recv(std::string&);

    string getName() const {
        return mName;
    };
    void addBuddy(const std::string& buddy);
    void removeBuddy(const std::string& buddy);
    bool isBuddy(const std::string& buddy) const;
    list<string> getBuddies() const;

    void bufferPacket(const std::string& packer);
    string getNextPacket();
private:
    string mName;
    BuddyList *mBuddyList;
    FifoList<string> *mPackets;
};

#endif	/* CLIENT_H */

