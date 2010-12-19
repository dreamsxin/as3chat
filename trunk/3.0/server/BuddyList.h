/* 
 * File:   BuddyList.h
 * Author: myleft
 *
 * Created on 2010年12月19日, 下午11:23
 */

#ifndef BUDDYLIST_H
#define	BUDDYLIST_H
#include <map>
#include <string>
#include <list>
using std::multimap;
using std::string;
using std::list;
class BuddyList {
public:
    BuddyList();
    void addBuddy(const string& name, const string& buddy);
    void removeBuddy(const string&name, const string& buddy);
    bool isBuddy(const string&name, const string& buddy) const;
    list<string> getBuddies(const string& name) const;
protected:
    multimap<string, string> mBuddies;
private:
    BuddyList(const BuddyList& src);
    BuddyList& operator=(const BuddyList& rhs);

};

#endif	/* BUDDYLIST_H */

