/* 
 * File:   RoomUserList.cpp
 * Author: Zhuzx
 * 
 * Created on 2011年2月11日, 上午10:07
 */

#include "RoomUserList.h"

RoomUserList::RoomUserList() {
}

RoomUserList::~RoomUserList() {
}

void RoomUserList::addUser(const string& room, const string& user) {
    //    if (!inRoom(room, user)) {
    //
    //    }
    //std::map<string, string>::iterator iter = mUserRoom.find(user);
    //if (iter != mUserRoom.end()) {
    //}

    if (mUserRoom.count(user) == 0) {

        std::pair < std::map<string, string>::iterator, bool> ret;
        ret = mUserRoom.insert(std::make_pair(user, room));
        if (!ret.second) {
            return;
        }
        mRoomUser.insert(std::make_pair(room, user));
    }
}

void RoomUserList::removeUser(const string& user) {
    std::map<string, string>::iterator iter;
    iter = mUserRoom.find(user);
    if (iter != mUserRoom.end()) {
        mUserRoom.erase(user);
        multimap<string, string>::iterator start, end;
        start = mRoomUser.lower_bound(iter->second);
        end = mRoomUser.upper_bound(iter->second);
        for (start; start != end; ++start) {
            if (start->second == user) {
                mRoomUser.erase(start);
                break;
            }
        }
    }
}

bool RoomUserList::inRoom(const string& user) const {
    return mUserRoom.count(user) == 0;
}

list<string> RoomUserList::getUsers() const {
    std::list<string> users;
    std::map<string, string>::const_iterator iter;
    for (iter = mUserRoom.begin(); iter != mUserRoom.end(); iter++) {
        users.push_back(iter->second);
    }
    return (users);
}

list<string> RoomUserList::getUsers(const string& room) const {
    std::pair<multimap<string, string>::const_iterator, multimap<string, string>::const_iterator> its;
    its = mRoomUser.equal_range(room);

    list<string> users;
    for (its.first; its.first != its.second; ++its.first) {
        users.push_back((its.first)->second);
    }
    return (users);
}
