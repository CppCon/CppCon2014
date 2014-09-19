//
// Morning puzzler: see last comment (please don't reveal answer
// until everyone has had a chance to think about  it...)

#include <iostream>
using namespace std;

class Base
{
    public:
        Base() {}
        virtual void func()
        {
            cout << "Base's func() now running\n";
        }
};

class Derived : public Base
{
    public:
        Derived() {}
        void func()
        {
            Base:func();    // this is a LABEL!
            cout << "Derived's func() now running\n";
        }
};

int main()
{
    Base *bp = new Derived;
    bp->func();     // What's the run-time result?
}                   // Result is infinite recursion and crash.
