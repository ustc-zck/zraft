#include "../src/socket.h"
#include <iostream>

int main(){
    Socket* s = new Socket();
    int ret;
    ret = s->SetAddrPortReuse();
    if (ret < 0) {
        return -1;
    }
    // ret = s->SetSocketNonBlocking();
    // if (ret < 0) {
    //     return -1;
    // }
    ret = s->Bind(9000);
    if (ret < 0){
        return -1;
    }
    ret = s->Listen();
    if (ret < 0 ){
        return -1;
    }
    int new_fd= s->Accept();
    if (new_fd < 0) {
        return -1;
    }

    Socket* new_socket = new Socket(new_fd);
    ret = new_socket->Recev();
    if (ret < 0){
        return -1;
    }
    std::cout << new_socket->ReadBuf() << std::endl;
    
    ret = new_socket->Send("Hello from server");
    if (ret < 0){
        return -1;
    }
    std::cout << "Hello message sent" << std::endl;
    
    return 0;
}