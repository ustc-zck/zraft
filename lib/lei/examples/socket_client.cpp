#include "../src/socket.h"
#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <unistd.h>

int proc(int n){
    sleep(10 - n);
    Socket* s = new Socket();
    int ret;
    ret = s->Connect("127.0.0.1:9000");
    if (ret < 0){
        return -1;
    }
    
    std::string str = "Hello World";
    char* msg = const_cast<char*>(str.c_str());
    ret = s->Send(msg);

    if (ret < 0){
        return -1;
    }
    //std::cout << "Hello message sent"  << std::endl;
    ret = s->Recev();
    if (ret < 0){
        return -1;
    }
    std::cout << s->ReadBuf() << std::endl;
    std::cout << std::this_thread::get_id() << std::endl;

    std::cout << n << std::endl;

    return 0;
}


int main(){
    int n = 10;
    std::vector<std::thread> threads(n);
    for (int i = 0; i < n; i++){
        threads[i] = std::thread(proc, i);
    }
    for(auto& thread : threads){
        thread.join();
    }
    return 0 ;
}