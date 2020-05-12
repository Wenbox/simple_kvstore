//
// Created by wxu on 12.05.20.
//

#ifndef SIMPLE_KVSTORE_CONFIG_H
#define SIMPLE_KVSTORE_CONFIG_H


#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

class config {
public:
    enum  {KEY_SIZE = 8, VALUE_SIZE=256};
    static int PORT() {
        return config_instance->port;
    }
    static bool ENABLE_PERSISTENCE() {
        return config_instance->enable_persistent;
    }
    static std::string BACKUP_FILE() {
        return config_instance->backup_file;
    }

    static void initialize_config(){
        if (config_instance == nullptr){
            config_instance = new config();
            load_config();
        }
    }
private:
    config() :
            port(1313),
            enable_persistent(false),
            backup_file("persistence.txt") {

    }

    static void load_config() {
        std::ifstream infile("../config.txt", std::ifstream::in);

        if(!infile.is_open()) {
            std::cout << "Cannot load config.txt, use default configuration instead." << std::endl;
            return;
        }
        std::cout << "Configuring...\n";
        std::string line;
        while(infile.good() && std::getline(infile, line)) {
            std::string key;
            std::string val;
            size_t i = line.find_first_not_of(" \f\n\r\t\v");
            size_t j = line.find_first_of(" \f\n\r\t\v=", i);
            if(i >= j)
                continue;
            key = line.substr(i, j - i);
            i = line.find_first_not_of(" \f\n\r\t\v", j+1);
            j = line.find_first_of(" \f\n\r\t\v", i);
            val = line.substr(i, j - i);
            if(key == "port") {
                config_instance->port = std::stoi(val);
                std::cout << "port number: " << config_instance->port << std::endl;
            } else if (key == "enable_persistent") {
                config_instance->enable_persistent = val == "yes" ? true : false;
                std::cout << "Enable data persistence: " << (val == "yes" ? "yes": "no") << std::endl;
            } else if(key == "path_to_backup_file") {
                config_instance->backup_file = val;
            }
        }
        if(config_instance->enable_persistent)
            std::cout << "Back up file: " << config_instance->backup_file << std::endl;

        infile.close();
    }

    static config* config_instance;
    int port;
    bool enable_persistent;
    std::string backup_file;
};


#endif //SIMPLE_KVSTORE_CONFIG_H
