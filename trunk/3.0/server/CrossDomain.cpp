/* 
 * File:   CrossDomain.cpp
 * Author: phpwind
 * 
 * Created on 2010年12月22日, 上午10:05
 */
#include <fstream>
#include "CrossDomain.h"
#include "Log.h"

CrossDomain::CrossDomain() {
}

CrossDomain::~CrossDomain() {
}

void CrossDomain::run() {
    int port = 843;
    std::string policy = "<policy-file-request/>";
    std::string crossdomain = "./crossdomain.xml";
    Epoll epoll;
    Socket socket;
    if (!socket.create()) {
        Log::error("file:%s, line:%d, CrossDomain socket.create() error\n", __FILE__, __LINE__);
        return;
    }
    if (!socket.bind(port)) {
        Log::error("file:%s, line:%d, CrossDomain socket.bind() error\n", __FILE__, __LINE__);
        return;
    }
    Log::debug("file:%s, line:%d, CrossDomain port:%d\n", __FILE__, __LINE__, port);
    if (!socket.listen()) {
        Log::error("file:%s, line:%d, CrossDomain socket.listen() error\n", __FILE__, __LINE__);
        return;
    }
    if (!socket.set_non_blocking(true)) {
        Log::error("file:%s, line:%d, CrossDomain socket.set_non_blocking()\n", __FILE__, __LINE__);
        return;
    }

    struct epoll_event events[1024];

    epoll_data_t epoll_data;
    epoll_data.fd = socket.getSock();
    epoll.Register(socket.getSock(), EPOLLIN, &epoll_data); //EPOLLONESHOT | EPOLLET

    int nfds, i;
    std::ifstream fin;
    while (true) {
        nfds = epoll.Select(events, sizeof (events), -1); // -1 10000毫秒
        for (i = 0; i < nfds; ++i) {
            struct epoll_event ev = events[i];
            if (ev.data.fd == socket.getSock()) {//主socket
                Log::debug("%s, %d, ACCEPT\n", __FILE__, __LINE__);
                int fd = socket.accept();
                if (fd <= 0) {
                    //cerr << "socket.accept(client) < 0" << endl;
                    continue;
                }
                Socket *client = new Socket(fd);

                client->set_non_blocking(true);

                epoll_data_t d;
                d.ptr = (void*) client;
                epoll.Register(client->getSock(), EPOLLIN | EPOLLET, &d);
                Log::debug("file:%s, line:%d, socket.accept(client)\n", __FILE__, __LINE__);
            } else if (ev.events & EPOLLIN) {
                // 接收数据
                Log::debug("%s, %d, EPOLLIN\n", __FILE__, __LINE__);
                if (ev.data.ptr == NULL) {
                    continue;
                }
                Socket *client = (Socket *) (ev.data.ptr);
                if (client->getSock() < 0) {
                    epoll.Unregister(client->getSock());
                    delete client;
                    client = NULL;
                    continue;
                }

                char buf [ Socket::MAXRECV + 1 ];
                memset(buf, '\0', sizeof (buf));
                int recv = client->recv(&buf, sizeof (buf));
                std::cout << "buf size:" << strlen(buf) << " recv:" << recv << std::endl;
                if (recv <= 0) {
                    Log::debug("%s, %d, recv <= 0\n", __FILE__, __LINE__);
                    epoll.Unregister(client->getSock());
                    delete client;
                    client = NULL;
                    continue;
                } else if (strncmp(buf, policy.c_str(), policy.size()) != 0) {
                    Log::debug("%s, %d, buf:%s\n", __FILE__, __LINE__, buf);
                    client->send(buf, strlen(buf));
                    epoll.Unregister(client->getSock());
                    delete client;
                    client = NULL;
                    continue;
                } else {
                    epoll_data_t d;
                    d.ptr = client;
                    epoll.Update(client->getSock(), EPOLLIN | EPOLLOUT | EPOLLET, &d);
                    continue;
                }
            } else if (ev.events & EPOLLOUT) {
                //写入数据
                Log::debug("%s, %d, EPOLLOUT\n", __FILE__, __LINE__);
                if (ev.data.ptr == NULL) {
                    continue;
                }
                Socket *client = (Socket *) (ev.data.ptr);
                if (client->getSock() < 0) {
                    epoll.Unregister(client->getSock());
                    delete client;
                    client = NULL;
                    continue;
                }
                Log::debug("%s, %d, EPOLLOUT\n", __FILE__, __LINE__);
                fin.open(crossdomain, std::ios::binary);
                if (fin.is_open()) {
                    char buf[Socket::MAXRECV + 1];
                    memset(buf, '\0', sizeof (buf));
                    while (!fin.eof()) {
                        fin.read(buf, sizeof (buf));
                        client->send(buf, fin.gcount());
                        memset(buf, '\0', sizeof (buf));
                        Log::debug("buf:%s, %s, %d", buf, __FILE__, __LINE__);
                    }
                    fin.close();
                    fin.clear();
                }

                Log::debug("%s, %d, EPOLLOUT\n", __FILE__, __LINE__);
                /*
                epoll_data_t d;
                d.ptr = client;
                epoll.Update(client->getSock(), EPOLLIN | EPOLLET, &d);
                 */
                epoll.Unregister(client->getSock());
                delete client;
                client = NULL;
                continue;
            } else {
                if (ev.data.ptr < 0) {
                    continue;
                }
                Socket *client = (Socket *) (ev.data.ptr);

                epoll.Unregister(client->getSock());

                delete client;
                client = NULL;
                std::cerr << "other event" << std::endl;
            }
        }
        Log::debug("%s, %d, 一次主循环结束\n", __FILE__, __LINE__);
    }
}
