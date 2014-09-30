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
    B b(A(i));  // ******: declaration of function: B b(A);

    b(A(5));    // Call the function
}

B b(A a)        // And here's an implementation...
{
    cout << "b() called!\n";
    return B(a);
}

// This analysis explains why the compiler sees that as a func. decl.:

int f(int x);  // a function taking an int
int f(int (x));  // redundant parentheses around parameter name OK; still
        // a declaration of a function f, not an int initialized by
        // static_cast<int>(x)


B b(A i);  // a function taking an A
B b(A (i));  // redundant parentheses around parameter name OK; still
        // a declaration of a function b, not a B object initialized by
        // an A constructed from i.

