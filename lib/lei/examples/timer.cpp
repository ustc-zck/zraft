#include <iostream>
#include <string> 
#include <thread>
#include "../src/server.h"


//g++ -o timer timer.cpp ../src/*cpp -lpthread

void Test1(){
    std::cout << "test timder event 1" << std::endl;
    sleep(10);
    //return 0;
}

void Test2(){
    std::cout << "test timder even 2" << std::endl;
    sleep(10);
    //return 0;
}

void Run(Server* s){
    s->Run();
}

int main(){
    Server* s1 = new Server(9000);
    // s1->TimerEvent(Test1, 100);
    // std::thread t1 = std::thread(Run, s1);
    std::thread t2 = std::thread(Test2);  
    // t1.join();
    t2.join();

    std::cout << "test thread" << std::endl;
    return 0;
}