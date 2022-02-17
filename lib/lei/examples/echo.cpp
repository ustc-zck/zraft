#include <iostream>
#include <string>
#include "../src/server.h"

std::string Echo(char* buf){
    std::cout << "server recevd msg:" << buf << std::endl;
    return buf;
}

int main(){
    Server* s = new Server(9000);
    s->Handler = &Echo;
    s->Run();
    return 0;
}

