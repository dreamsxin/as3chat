/* 
 * File:   MessageTypes.h
 * Author: phpwind
 *
 * Created on 2010年12月22日, 下午3:33
 */

#ifndef MESSAGETYPES_H
#define	MESSAGETYPES_H

class MessageType {
public:
    //static const int EV_OTHER;

    enum {
        EV_TYPE_PING = 0,
        EV_TYPE_AUTH,
        EV_TYPE_AUTH_ANONYMOUS,
        EV_TYPE_AUTH_OTHER_LOGIN,
        EV_TYPE_AUTH_FAIL,
        EV_TYPE_USER_LOGIN,
        EV_TYPE_USER_LOGOUT,
        EV_TYPE_USER_JOIN_ROOM,
        EV_TYPE_USER_QUIT_ROOM,
        EV_TYPE_USER_CHANGE_STATE,
        EV_TYPE_USER_LIST,
        EV_TYPE_USER_ADD,
        EV_TYPE_USER_CUT,
        EV_TYPE_MESSAGE_SYSTEM,
        EV_TYPE_MESSAGE_PUBLIC,
        EV_TYPE_MESSAGE_PRIVATE,
        EV_TYPE_ROOM_LIST,
        EV_TYPE_ROOM_ADD,
        EV_TYPE_ROOM_CUT,
        EV_TYPE_ADMIN_COMMAND_GOOUT,
        EV_TYPE_ADMIN_COMMAND_SHUTUP
    };
};

#endif	/* MESSAGETYPES_H */

