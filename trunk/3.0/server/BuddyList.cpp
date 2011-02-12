/* 
 * File:   BuddyList.cpp
 * Author: myleft
 * 
 * Created on 2010年12月19日, 下午11:23
 */

#include "BuddyList.h"

BuddyList::BuddyList() {
}

BuddyList::~BuddyList() {
}

bool BuddyList::addBuddy(const string& group, const string& buddy) {
    if (mBuddyGroup.count(buddy) == 0) {
        std::pair < std::map<string, string>::iterator, bool> ret;
        ret = mBuddyGroup.insert(std::make_pair(buddy, group));
        if (ret.second) {
            mGroupBuddy.insert(make_pair(group, buddy));
            return true;
        }
    }
    return false;
}

bool BuddyList::removeBuddy(const string& buddy) {
    std::map<string, string>::iterator iter;
    iter = mBuddyGroup.find(buddy);
    if (iter != mBuddyGroup.end()) {
        multimap<string, string>::iterator start, end;
        start = mGroupBuddy.lower_bound(iter->second);
        end = mGroupBuddy.upper_bound(iter->second);
        for (start; start != end; ++start) {
            if (start->second == buddy) {
                mGroupBuddy.erase(start);
                return true;
            }
        }
    }
    return false;
}

bool BuddyList::isBuddy(const string& buddy) const {
    return mBuddyGroup.count(buddy) == 0;
}

list<string> BuddyList::getBuddies() const {
    std::list<string> buddies;
    std::map<string, string>::const_iterator iter;
    for (iter = mBuddyGroup.begin(); iter != mBuddyGroup.end(); iter++) {
        buddies.push_back(iter->second);
    }
    return (buddies);
}

list<string> BuddyList::getBuddies(const string& group) const {
    std::pair<std::multimap<string, string>::const_iterator, std::multimap<string, string>::const_iterator> its;
    its = mGroupBuddy.equal_range(group);

    std::list<string> buddies;
    for (its.first; its.first != its.second; ++its.first) {
        buddies.push_back((its.first)->second);
    }
    return (buddies);
}
