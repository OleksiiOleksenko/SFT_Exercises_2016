/* array read out of bounds */
#include <stdio.h>
#include <stdlib.h>

int g[10];

int main(int argc, char **argv) {
  int i;
  for (i=0; i<10; i++)
    g[i] = 42;
  i = atoi(argv[1]);
  int x = g[i];
  return x;
}
