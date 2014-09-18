#ifdef __clang__
#include <stdio.h>
#else
// GCC 4.8 doesn't know that puts() is nothrow; we must give it a hint.
extern void puts(const char*) noexcept(true);
#endif

#include "auto.h"

extern void foo();

int main() {
  if (true) {
    Auto(puts("two"));
    puts("one");          // compiler knows this doesn't throw
  }
  if (true) {
    Auto(puts("three"));
    foo();                // might throw an exception
  }
}

