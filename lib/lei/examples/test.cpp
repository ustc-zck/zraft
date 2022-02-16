#include <iostream>
#include <string.h>

// char* Echo(char* buf){
//     return buf;
// }


// int main(){
//     char* (*Handler)(char* buf);
//     Handler = Echo;
//     std::cout << Handler("123") << std::endl;
    
//     return 1;
// }

int main(){
    char a[1024];
    strcpy(a, "1111");
    std::cout << sizeof(a) << std::endl;
    std::cout << strlen(a) << std::endl;
    a[2] = '\0';
    std::cout << strlen(a) << std::endl;
    return 0;
}