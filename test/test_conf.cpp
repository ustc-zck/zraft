#include "../src/conf.h"

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
    std::cout << "term period is: " << conf->termPeriod << std::endl;
    
    return 0;
}