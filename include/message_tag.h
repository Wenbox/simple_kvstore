//
// Created by wxu on 08.05.20.
//

#ifndef SIMPLE_KVSTORE_MESSAGE_H
#define SIMPLE_KVSTORE_MESSAGE_H


#include <vector>

enum message_tag : char {
        //reply
        OK = 0,
        NOT_FOUND,
        UNKNOWN_OP,
        ILLEGAL_PARA,
        ERROR,
        //request
        PUT = 'a',
        GET,
        DELETE,
        SCAN,
};


#endif //SIMPLE_KVSTORE_MESSAGE_H
