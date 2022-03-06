#include <iostream>
#include <string> 
#include "../src/server.h"


//g++ -o timer timer.cpp ../src/*cpp -lpthread

void Test(){
    std::cout << "test timder event 00" << std::endl;
    sleep(10);
    //return 0;
}

void Test1(){
    std::cout << "test timder even 01" << std::endl;
    //sleep(10);
    //return 0;
}

int main(){
    Server* s = new Server(9000);
    s->TimerEvent(Test, 100);
    Server* s1 =  new Server(9001);
    s1->TimerEvent(Test1, 100);
    s->Run();
    return 0;
    return 0;
}