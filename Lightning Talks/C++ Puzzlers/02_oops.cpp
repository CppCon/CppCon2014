// Puzzler: see comment at bottom for the question
// (PLEASE don't say anything about the REASON, just answer the question!)

#include <iostream>
using namespace std;

class Base
{
    public:
        virtual void func()
        {
            cout << "Base's func() now running\n";
        }
};

class Derived : public Base
{
    public:
        void func()
        {
            Base:func();
            cout << "Derived's func() now running\n";
        }
};

int main()
{
    Base *bp = new Derived;
    bp->func();				// Question: What's the runtime behavior?
}
