//
// Puzzler
//

#include <iostream>
using namespace std;

class A
{
    public:
        A (int i) {}
};

class B {
    public:
        B (A a) {}
};

int main()
{
    int i = 1;
    B b(A(i));  // what is b's type?
}

