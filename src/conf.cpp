#include "conf.h"
#include <fstream>

RaftConf::RaftConf(std::string confPath){
    std::ifstream conf(confPath);
    std::string line;
    if(!conf.is_open()){
        abort();
    }
    while(getline(conf, line)){
        std::cout << line << std::endl;
    }
    conf.close();
}