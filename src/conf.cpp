#include "conf.h"
#include <fstream>
#include <boost/algorithm/string.hpp>

RaftConf::RaftConf(std::string confPath){
    std::ifstream conf(confPath);
    std::string line;
    if(!conf.is_open()){
        abort();
    }
    while(getline(conf, line)){
        std::size_t pos = line.find(":");
        if(pos == std::string::npos){
            continue;
        }
        std::string key = line.substr(0, pos);
        boost::trim_right(key);
        boost::trim_left(key);

        std::string val = line.substr(pos + 1);
        boost::trim_right(val);
        boost::trim_left(val);

        
        if(key == "addr"){
            addr = val;
        } else if(key == "peers"){
            if(val.find(",") == std::string::npos){
                peers.push_back(val);
            } else{
                int startPos = 0;
                for(int i = 0; i < val.length(); i++){
                    if(val[i] == ','){
                        std::string peer = val.substr(startPos, i - startPos);
                        peers.push_back(peer);
                        startPos = i + 1;
                    }
                }
                peers.push_back(val.substr(startPos));
            }

        } else if(key == "election_time_out"){
            electionTimeOut =  std::stol(val);
        } else if(key == "broadcast_time_out"){
            broadcastTimeOut = std::stol(val);

        } else if(key == "term_period"){
            termPeriod = std::stol(val);
        } else{
            //TODO...
        }
    }
    conf.close();
}
