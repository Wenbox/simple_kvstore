//
// Created by wxu on 10.05.20.
//

#ifndef SIMPLE_KVSTORE_KV_STORE_CONFIG_H
#define SIMPLE_KVSTORE_KV_STORE_CONFIG_H
class kv_store_config {
public:
    enum  {KEY_SIZE = 8, VALUE_SIZE=256};
    static std::string PERSISTENCE_FILE;
    static bool ENABLE_PERSISTENCE;
    static int PORT;
};


#endif //SIMPLE_KVSTORE_KV_STORE_CONFIG_H
