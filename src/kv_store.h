//
// Created by wxu on 09.05.20.
//

#ifndef SIMPLE_KVSTORE_KV_STORE_H
#define SIMPLE_KVSTORE_KV_STORE_H

#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <fstream>
#include "config.h"

class kv_store {
public:
    kv_store() : m_enable_persistency(config::ENABLE_PERSISTENCE()) {
        if(m_enable_persistency) {
            restore_from_log();
        }
    }

    std::string get(std::string& key) {
        auto itr = m_map.find(key);
        if(itr == m_map.end())
            return "";
        else
            return itr->second;
    }
    void put(std::string& key, std::string& value) {
       if(m_enable_persistency) {
            try{
                std::ofstream outfile;
                outfile.open(config::BACKUP_FILE(), std::ios_base::app);
                outfile << "p" << key << value;
                outfile.close();

            } catch (std::exception) {
                throw;
            }
        }
        m_map[key] = value;

    }

    void erase(std::string& key) {
        if(m_map.count(key) > 0) {
            if (m_enable_persistency) {
                try {
                    std::ofstream outfile;
                    outfile.open(config::BACKUP_FILE(), std::ios_base::app);
                    outfile << "d" << key;
                    outfile.close();

                } catch (std::exception) {
                    throw;
                }
            }
            m_map.erase(key);
        }
    }

    std::string scan(std::string& lb, std::string& ub) {
        std::string res;
        auto itr = m_map.lower_bound(lb);
        while(itr != m_map.end() && itr->first <= ub) {
            res += itr->first;
            res += itr->second;
            ++itr;
        }
        return res;
    };

private:
    void restore_from_log() {
        try {
            std::cout << "Restoring kv-store from " << config::BACKUP_FILE() << std::endl;
            std::ifstream infile(config::BACKUP_FILE(), std::ifstream::binary);

            std::vector<char> buffer(256, 0);
            while(infile.read(buffer.data(), 1)) {
                if(buffer[0] == 'p') {
                    infile.read(buffer.data(), config::KEY_SIZE);
                    std::string key(buffer.begin(), buffer.begin() + config::KEY_SIZE);
                    infile.read(buffer.data(), config::VALUE_SIZE);
                    m_map[key] = std::string(buffer.begin(), buffer.begin() + config::VALUE_SIZE);
                } else if(buffer[0] == 'd') {
                    infile.read(buffer.data(), config::KEY_SIZE);
                    std::string key(buffer.begin(), buffer.begin() + config::KEY_SIZE);
                    m_map.erase(key);
                } else {
                    throw 13;
                }
            }


        } catch(...) {
            std::cerr << "Unable to restore, will use a brand new kv-store\n";
        }
    }
    bool m_enable_persistency;
    std::map<std::string, std::string> m_map;
};


#endif //SIMPLE_KVSTORE_KV_STORE_H
