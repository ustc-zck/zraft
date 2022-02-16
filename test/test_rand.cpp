#include <time.h> 
#include <iostream> 
using namespace std; 
int main() 
{ 
    srand( (unsigned)time( NULL ) ); 
 
    cout << (float) rand()/RAND_MAX << endl; 
} 