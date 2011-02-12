/* 
 * File:   parseMessage.cpp
 * Author: phpwind
 * 
 * Created on 2010年12月21日, 下午1:06
 */

#include "ParseMessage.h"

ParseMessage::ParseMessage(Client *client, Epoll &epoll, ClientList& clientList) {
    mClient = client;
    mEpoll = epoll;
    mClientList = &clientList;
}

ParseMessage::~ParseMessage() {
}

void ParseMessage::run() {
    if (mClient == NULL) {
        return;
    }
    TiXmlDocument *doc = new TiXmlDocument();
    TiXmlDocument *myDocument = new TiXmlDocument();
    try {
        std::string message = mClient->getNextInPacket();
        if (message.empty()) {
            return;
        }

        doc->Parse(message.c_str());
        if (doc->Error()) {
            throw (std::invalid_argument(doc->ErrorDesc()));
        }

        TiXmlHandle docHandle(doc);

        TiXmlElement* event = docHandle.FirstChild("event").ToElement();
        if (!event) {
            throw (std::invalid_argument("event not found"));
        }
        int eType = -1;
        event->Attribute("type", &eType);

        std::cout << "event value:" << event->Value() << std::endl;
        std::cout << "event type:" << eType << std::endl;
        if (eType < 0) {
            throw (std::invalid_argument("type type value < 0"));
        }

        TiXmlElement *RootElement = new TiXmlElement("event");
        myDocument->LinkEndChild(RootElement);
        TiXmlPrinter printer;
        switch (eType) {
            case MessageType::EV_TYPE_PING:
                RootElement->SetAttribute("type", MessageType::EV_TYPE_PING);
                myDocument->Accept(&printer);
                mClient->pushOutPacket(printer.CStr());
                break;
            case MessageType::EV_TYPE_AUTH:
                RootElement->SetAttribute("type", MessageType::EV_TYPE_AUTH_FAIL);
                myDocument->Accept(&printer);
                mClient->pushOutPacket(printer.CStr());
                break;
            case MessageType::EV_TYPE_AUTH_ANONYMOUS:
                RootElement->SetAttribute("type", MessageType::EV_TYPE_AUTH_FAIL);
                myDocument->Accept(&printer);
                mClient->pushOutPacket(printer.CStr());
                break;
            case MessageType::EV_TYPE_MESSAGE_PUBLIC:
            {
                RootElement->SetAttribute("type", MessageType::EV_TYPE_MESSAGE_PUBLIC);
                TiXmlElement* body = docHandle.FirstChildElement("event").FirstChildElement("body").Element();
                if (body) {
                    RootElement->LinkEndChild(body);
                }

                myDocument->Accept(&printer);
                mClient->pushOutPacket(printer.CStr());

                break;
            }
            default:
                Log::debug("%s, %d, eType not switch\n", __FILE__, __LINE__);
                break;
        }

        epoll_data_t d;
        d.ptr = (void*) mClient;
        mEpoll.Update(mClient->getSock(), EPOLLIN | EPOLLOUT | EPOLLET, &d);
    } catch (...) {
        Log::debug("%s, %d, ParseMessage fail\n", __FILE__, __LINE__);
        mClientList->removeClient(mClient);
    }
    delete myDocument;
    delete doc;
}

