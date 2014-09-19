// Not a "puzzler" in the usual sense, but worth thinking about:
// Inline member functions should always be defined in
// header files...but there are 3 totally equivalent (and legal)
// ways to do this, syntactically.
//
// Are there any reasons to prefer one style above another?
// (THERE IS NO DIFFERENCE in semantics or performance!)
//

#include <iostream>					// Note: This is ALL in a header file
using std::cout;

class T
{
public:
    void foo()
        {
        cout << "foo()\n";              // option #1: "in-situ" (complete)
        }

    inline void bar();                  // option #2 (declaration)

    void zot();                         // option #3 (declaration)
};

void T::bar() { cout << "bar()\n"; }    // option #2 (definition)

inline void T::zot() {
    cout << "zot()\n"; }                // option #3 (definition)
