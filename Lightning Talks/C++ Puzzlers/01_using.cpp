// NOTE: There are TWO PARTS to this puzzler!

#include <iostream>

int main()  // Part I: Legal, or compile error?
{
    using namespace std;
    
    cout << "Hello, puzzling world!" << endl;

    int cout;
    cout = 10;
    std::cout << "cout = " << cout << endl;
    std::cout << "cout << 5 = " << (cout << 5) << endl;
}


// Part II: What if we replaced
//      using namespace std;
// with:
//      using std::cout;
//      using std::endl;
//
//  ?
