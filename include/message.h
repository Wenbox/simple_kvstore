//
// Created by wxu on 08.05.20.
//

#ifndef SIMPLE_KVSTORE_MESSAGE_H
#define SIMPLE_KVSTORE_MESSAGE_H


#include <vector>

class message {
public:

    enum tag : char
    {

        //request
        PUT = 'a',
        GET,
        DELETE,
        SCAN,
        NOT_FOUND,
        UNKNOWN_OP,
        ERROR


    };

    tag get_tag();
private:
    tag m_tag;
};


#endif //SIMPLE_KVSTORE_MESSAGE_H
