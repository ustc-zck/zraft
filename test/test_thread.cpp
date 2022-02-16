#include <thread>
#include <iostream>

void a(){
    while(1){
        std::cout << 1 << std::endl;
    }
}

void b(){
    while(1){
        std::cout << 2 << std::endl;
    }
}

int main(){
    std::thread thread1(a);
    std::thread thread2(b);
    thread1.join();
    thread2.join();
    return 0;
}