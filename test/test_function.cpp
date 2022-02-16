#include <functional>
#include <iostream>

using std::placeholders::_1;

class A{
public:
    std::function<int(const int&)> fun;
};

class B {
    public:
        B(){
            a = new A();
        }
        ~B(){
            delete a;
        }
        int test(const int& a){
            std::cout << "arg is " << a  << std::endl;
            return 0;
        }

        void run(){
            a->fun = std::bind(&B::test, this, _1);
            a->fun(1);
        }

        A* a;
};

int main(){
    B* b = new B();
    b->run();
    return 0;
}   

