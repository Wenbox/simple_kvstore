//
// Created by wxu on 08.05.20.
//

#ifndef SIMPLE_KVSTORE_MESSAGE_H
#define SIMPLE_KVSTORE_MESSAGE_H

enum message_tag : char {
        //reply
        OK,
        NOT_FOUND,
        UNKNOWN_OP,
        ILLEGAL_PARA,
        ERROR,
        //request
        PUT,
        GET,
        DELETE,
        SCAN,
};


#endif //SIMPLE_KVSTORE_MESSAGE_H
