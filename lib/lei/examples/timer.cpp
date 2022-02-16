#include <iostream>
#include <string> 
#include "../src/server.h"

int Test(){
    std::cout << "test timder fd" << std::endl;
    return 0;
}

int main(){
    Server* s = new Server(9000);
    s->TimeHandler = &Test;
    s->AddTimeEvent(200);
    s->Run();
    return 0;
    return 0;
}