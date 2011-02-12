/* 
 * File:   KeepAlive.cpp
 * Author: phpwind
 * 
 * Created on 2010年12月22日, 上午11:08
 */

#include "KeepAlive.h"

KeepAlive::KeepAlive(ClientList & clientList) {
    mClientList = &clientList;
}

KeepAlive::~KeepAlive() {
}

void KeepAlive::run() {
    while (true) {
        ::sleep(60);
        mClientList->keepAlive();
    }
}