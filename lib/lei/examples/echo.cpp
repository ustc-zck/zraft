#include <iostream>
#include <string>
#include "../src/server.h"

std::string Echo(char* buf){
    std::cout << "server recevd msg:" << buf << std::endl;
    return buf;
}

//g++ -o echo echo.cpp ../src/socket.h ../src/socket.cpp ../src/server.h ../src/server.cpp
int main(){
    Server* s = new Server(9001);
    s->Handler = &Echo;
    s->Run();
    return 0;
}

