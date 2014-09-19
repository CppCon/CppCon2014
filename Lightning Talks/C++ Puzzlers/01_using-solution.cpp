#include <iostream>

int main()  // Part I: Legal, or compile error?
{			// LEGAL! 
    using namespace std;
    
    cout << "hello, cruel world!" << endl;	// usual "cout"

    int cout;	// this cout HIDES the one at namespace scope
    cout = 10;	// uses the int
    std::cout << "cout = " << cout << endl;	// 2nd cout is the int
    std::cout << "cout << 5 = " << (cout << 5) << endl;	// 2nd cout is int
}


// Part II: What if we replaced
//      using namespace std;
// with:
//      using std::cout;
//      using std::endl;
//
//  ?

// Answer: ILLEGAL! A using DECLARATION brings the name into THIS scope,
// so you can't have a redeclaration in the same scope.
