#include <stdio.h>
#include <stdbool.h>

int main(int argc, char const *argv[])
{
  int a = false ? 123 : 245;
  printf("%d\n", a);
  int *p = &a;

  printf("sizeof int: %zu bytes\n", sizeof(a));
  printf("sizeof char: %zu bytes\n", sizeof(char));
  printf("%d\n", p);
  return 0;
}
