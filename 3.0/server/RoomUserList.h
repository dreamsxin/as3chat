/* 
 * File:   RoomUserList.h
 * Author: Zhuzx
 *
 * Created on 2011年2月11日, 上午10:07
 */

#ifndef ROOMUSERLIST_H
#define	ROOMUSERLIST_H
#include <string>
#include <map>
#include <list>
using std::multimap;
using std::string;
using std::list;

class RoomUserList {
public:
    RoomUserList();
    virtual ~RoomUserList();

    void addUser(const string& room, const string& user);
    void removeUser(const string& user);
    bool inRoom(const string& user) const;
    std::list<string> getUsers() const;
    std::list<string> getUsers(const string& room) const;

protected:
    std::map<string, string> mUserRoom;
    std::multimap<string, string> mRoomUser;

private:
    RoomUserList(const RoomUserList& src);
    RoomUserList & operator=(const RoomUserList& rhs);

};

#endif	/* ROOMUSERLIST_H */

