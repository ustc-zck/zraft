#include "../src/log.h"
#include <iostream>

//complie command line: g++ -o test_log test_log.cpp ../src/log.h ../src/log.cpp  -lboost_filesystem -lboost_system
int main(){
    RaftLog* log = new RaftLog("../log/");
    uint64_t i = 0; 
    while(i < 786432 + 10){
        std::string s;
        log->Log(i, "1\t1\tPUT\tHELLO\tWORLD\t");
        i++;
    }
    delete log;
    
    return 0;
}