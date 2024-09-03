#include <stdio.h>

int main(int argc, char const *argv[]) {
  int a = 1;
  int b = 2;
  int c = 3;
  int *p = &a;

  printf("b: %d\n", b);
  *(p + 1) = 4;
  printf("b: %d\n", b);

  printf("c: %d\n", c);
  *(p + 2) = 5;
  printf("c: %d\n", c);

  printf("Address of a: %p\n", &a);
  printf("Address of b: %p\n", &b);
  printf("Address of c: %p\n", &c);

  return 0;
}
