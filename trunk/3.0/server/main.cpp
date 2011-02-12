/* 
 * File:   main.cpp
 * Author: phpwind
 *
 * Created on 2010年12月16日, 上午9:52
 */

#include <cstdlib>

#include <iostream>
#include <vector>
#include <assert.h>
#include <string.h>  // strstr() <cstring>
#include <stdarg.h>

#include <fstream>

#include "Log.h"
#include "squirrel/squirrel.h"
#include "squirrel/sqstdio.h"
#include "squirrel/sqstdaux.h"

#include "tinyxml/tinyxml.h"
#include "Epoll.h"
#include "Socket.h"
#include "Client.h"
#include "ClientList.h"
#include "FifoList.h"
#include "BuddyList.h"
#include "ThreadPool.h"
#include "CrossDomain.h"
#include "KeepAlive.h"
#include "RecvMessage.h"
using namespace std;

/*
 *
 */

int main(int argc, char** argv) {

    //客户列表
    ClientList clientlist;
    
    //房间列表
    //RoomList roomlist;
    
    // 安全验证
    ThreadPool policypool(1);
    policypool.Start();
    policypool.ExecuteAsync(new CrossDomain());

    // 心跳包
    ThreadPool keepalivepool(1);
    keepalivepool.Start();
    keepalivepool.ExecuteAsync(new KeepAlive(clientlist));

    // 线程池
    ThreadPool pool(5);
    pool.Start();
    cout << "ThreadPool start" << endl;

    // 配置参数
    int port = 8080;

    Epoll epoll;

    Socket socket;
    if (!socket.create()) {
        Log::error("file:%s, line:%d, socket.create()\n", __FILE__, __LINE__);
        return 1;
    }
    if (!socket.bind(port)) {
        Log::error("file:%s, line:%d, socket.bind()\n", __FILE__, __LINE__);
        return 1;
    }
    Log::debug("file:%s, line:%d, server port:%d\n", __FILE__, __LINE__, port);
    if (!socket.listen()) {
        Log::error("file:%s, line:%d, socket.listen()\n", __FILE__, __LINE__);
        return 1;
    }
    if (!socket.set_non_blocking(true)) {
        Log::error("file:%s, line:%d, socket.set_non_blocking()\n", __FILE__, __LINE__);
        return 1;
    }

    // 初始化线程

    struct epoll_event events[1024];

    epoll_data_t epoll_data;
    epoll_data.fd = socket.getSock();
    epoll.Register(socket.getSock(), EPOLLIN, &epoll_data); //EPOLLONESHOT | EPOLLET

    int nfds, i;
    while (true) {
        nfds = epoll.Select(events, sizeof (events), -1); // -1 60000毫秒
        for (i = 0; i < nfds; ++i) {
            //Log::debug("file:%s, line:%d\n", __FILE__, __LINE__);
            struct epoll_event ev = events[i];
            if (ev.data.fd == socket.getSock()) {//主socket
                Log::debug("%s, %d, ACCEPT\n", __FILE__, __LINE__);
                int fd = socket.accept();
                if (fd <= 0) {
                    cerr << "socket.accept(client) < 0" << endl;
                    continue;
                }
                Client *client = new Client(fd);
                if (clientlist.addClient(client)) {
                    client->set_non_blocking(true);

                    epoll_data_t d;
                    d.ptr = (void*) client;
                    epoll.Register(client->getSock(), EPOLLIN | EPOLLET, &d);
                } else {
                    delete client;
                    client = NULL;
                }
                Log::debug("file:%s, line:%d, socket.accept(client)\n", __FILE__, __LINE__);
            } else if (ev.events & EPOLLIN) {
                // 接收数据
                Log::debug("%s, %d, EPOLLIN\n", __FILE__, __LINE__);
                if (ev.data.ptr == NULL) {
                    Log::debug("%s, %d, ev.data.ptr == NULL\n", __FILE__, __LINE__);
                    continue;
                }
                Client *client = (Client *) (ev.data.ptr);
                if (client->getSock() < 0) {
                    epoll.Unregister(client->getSock());
                    clientlist.removeClient(client);
                    //ev.data.ptr = NULL;
                    Log::debug("%s, %d, removeClient\n", __FILE__, __LINE__);
                    continue;
                }

                pool.ExecuteAsync(new RecvMessage(client, epoll, pool, clientlist));
                continue;
            } else if (ev.events & EPOLLOUT) {
                //写入数据
                Log::debug("%s, %d, EPOLLOUT\n", __FILE__, __LINE__);
                if (ev.data.ptr == NULL) {
                    continue;
                }
                Client *client = (Client *) (ev.data.ptr);
                if (client->getSock() < 0) {
                    epoll.Unregister(client->getSock());
                    clientlist.removeClient(client);
                    //ev.data.ptr = NULL;
                    continue;
                }
                try {
                    string message = client->getNextOutPacket();
                    if (!message.empty()) {
                        int status = client->send(message);
                        Log::debug("%s, %d, send message status:%d\n", __FILE__, __LINE__, status);
                    }
                    epoll_data_t d;
                    d.ptr = client;
                    epoll.Update(client->getSock(), EPOLLIN | EPOLLET, &d);
                } catch(...) {
                    Log::debug("%s, %d, send message error\n", __FILE__, __LINE__);
                    epoll.Unregister(client->getSock());
                    clientlist.removeClient(client);
                }
                continue;
            } else if (ev.events & EPOLLRDHUP) {//tcp连接中断检测
                Log::debug("%s, %d, EPOLLRDHUP\n", __FILE__, __LINE__);
                if (ev.data.ptr == NULL) {
                    continue;
                }
                Client *client = (Client *) (ev.data.ptr);
                epoll.Unregister(client->getSock());
                delete client;
                client = NULL;
                //ev.data.ptr = NULL;
                continue;
            } else if (ev.events & EPOLLERR) {//tcp连接中断检测
                Log::debug("%s, %d, EPOLLERR\n", __FILE__, __LINE__);
                if (ev.data.ptr == NULL) {
                    continue;
                }
                Client *client = (Client *) (ev.data.ptr);

                epoll.Unregister(client->getSock());

                delete client;
                client = NULL;
                //ev.data.ptr = NULL;
                continue;
            } else {
                if (ev.data.ptr < 0) {
                    continue;
                }
                Client *client = (Client *) (ev.data.ptr);

                epoll.Unregister(client->getSock());

                delete client;
                client = NULL;
                //ev.data.ptr = NULL;
                cerr << "other event" << endl;
            }
            cout << "一次循环结束" << endl;

        }
    }
    return 0;
}
