# lei
A simple net/tcp library using Reactor model, supporting IO event and timer event. Thread pool is used to improve performance. 

#EXAMPLE

A simple example is shown that how to use the lei library

```
#include <iostream>
#include "../src/server.h"

//define a IO handler...
char* Echo(char* buf){
    std::cout << "server recevd msg:" << buf << std::endl;
    return buf;
}

int main(){
    //default port 8080
    Server* s = new Server(9000);
    s->Handler = &Echo;
    s->Run();
    return 0;
}
```

To run a timer event in loop, just need to call AddTimeEvent function

```
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
```


#TODO 

Now, signal event is not supported in this library, which will be added in the future.
