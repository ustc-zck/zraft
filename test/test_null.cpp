#include <iostream>
#include <string>

int main(){
    std::string* s;
    s = nullptr;
    if(s == nullptr){
        std::cout << "test null" << std::endl;
    }    
    return 0;
}