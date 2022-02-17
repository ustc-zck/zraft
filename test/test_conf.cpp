#include "../src/conf.h"


//complie command line: g++ -o test_conf test_conf.cpp ../src/conf.h ../src/conf.cpp 

int main(){
    RaftConf* conf = new RaftConf();
    std::cout << "addr is: " << conf->addr << std::endl;
    std::cout << "peer addrs are: ";
    for(auto peer : conf->peers){
        std::cout << peer << "\t";
    }
    std::cout << std::endl;

    std::cout << "election time out is: " << conf->electionTimeOut << std::endl;
    std::cout << "broadcast time out is: " << conf->broadcastTimeOut << std::endl;
    std::cout << "term time out is: " << conf->termTimeOut<< std::endl;
    std::cout << "port is " << conf->port << std::endl;
    
    return 0;
}