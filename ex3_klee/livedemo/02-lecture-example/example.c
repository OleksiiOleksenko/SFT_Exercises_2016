#include "klee/klee.h"

int bad_abs(int x)
{
 if (x < 0)
   return -x;
 if (x == 1234)
   return -x;
 return x;
}

int main() {
  int x;
  klee_make_symbolic(&x, sizeof(x), "x");
  return bad_abs(x);
}
