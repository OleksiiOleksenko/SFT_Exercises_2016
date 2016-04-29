#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

unsigned char calc(unsigned char op, unsigned char x, unsigned char y) {
  unsigned char r;
  assert(op == '+' || op == '-' || op == '*' || op == '/');

  if (op == '+')  r = x + y;
  if (op == '-')  r = x - y;
  if (op == '*')  r = x * y;
  if (op == '/')  r = x / y;
  return r;
}

int main(int argc, char** argv) {
  unsigned char r;
  unsigned char op = argv[1][0];
  unsigned char x  = atoi(argv[2]);
  unsigned char y  = atoi(argv[3]);

  r = calc(op, x, y);
  printf("result = %d\n", r);

  return 0;
}
