//
// Created by wxu on 09.05.20.
//

#ifndef SIMPLE_KVSTORE_KV_STORE_H
#define SIMPLE_KVSTORE_KV_STORE_H

#include <string>
#include <vector>
#include <map>

class kv_store {
public:
    enum  {KEY_SIZE = 8, VALUE_SIZE=256};
    std::string get(std::string& key) {
        auto itr = m_map.find(key);
        if(itr == m_map.end())
            return "";
        else
            return itr->second;
    }
    void put(std::string& key, std::string& value) {
        m_map[key] = value;
        std::cout << "PUT " << key << " : " << value << std::endl;
    }

    void erase(std::string& key) {
        m_map.erase(key);
    }

    std::string scan(std::string& lb, std::string& ub) {
        std::string res;
        auto itr = m_map.lower_bound(lb);
        while(itr != m_map.end() && itr->first <= ub) {
            res += itr->first;
            res += itr->second;
            std::cout << itr->first << " : " << itr->second << std::endl;
            ++itr;
        }
        return res;
    };

private:
    std::map<std::string, std::string> m_map;
};


#endif //SIMPLE_KVSTORE_KV_STORE_H
