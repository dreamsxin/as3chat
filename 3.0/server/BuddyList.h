/* 
 * File:   BuddyList.h
 * Author: myleft
 *
 * Created on 2010年12月19日, 下午11:23
 */

#ifndef BUDDYLIST_H
#define	BUDDYLIST_H
#include <string>
#include <map>
#include <list>
using namespace std;
class BuddyList {
public:
    BuddyList();
    virtual ~BuddyList();
    
    bool addBuddy(const string& group, const string& buddy);
    bool removeBuddy(const string& buddy);
    bool isBuddy(const string& buddy) const;
    list<string> getBuddies() const;
    list<string> getBuddies(const string& group) const;
protected:
    map<string, string> mBuddyGroup;
    multimap<string, string> mGroupBuddy;
private:
    BuddyList(const BuddyList& src);
    BuddyList& operator=(const BuddyList& rhs);

};

#endif	/* BUDDYLIST_H */

