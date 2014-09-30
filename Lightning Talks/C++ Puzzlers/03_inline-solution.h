// T.h:
// Which do think is best?
//
// ANSWER: I personally prefer option #3, because the INTERFACE
//      should not traffic in IMPLEMENTATION DETAILS...and whether
//      any particular function is inline or not is NOT part of
//      the interface a client has any need to know or care about.

#include <iostream>
using std::cout;

class T
{
public:
    void foo()
        {
        cout << "foo()\n";              // option #1: "in-situ"
        }

    inline void bar();                  // option #2

    void zot();                         // option #3
};

void T::bar() { cout << "bar()\n"; }    // option #2

inline void T::zot() {
    cout << "zot()\n"; }                // option #3
