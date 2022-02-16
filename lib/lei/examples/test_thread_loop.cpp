#include <iostream>
#include <vector>
#include <thread>

int test(int n){
    std::cout << n << std::endl;
    return 0;
}

int main(){
    std::thread th;
    std::cout << th.joinable() << std::endl;

    th = std::thread(test, 100);
    //seprate func from thread...
    th.detach();
    th = std::thread(test, 101);

    std::cout << th.joinable() << std::endl;
    th.join();

return 0;
}