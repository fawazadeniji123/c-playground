#include <stdio.h>

int main(int argc, char const *argv[])
{
  char a = 0b00000010;
  char b = 0b00000011;
  char c;

  puts("char a = 0b00000010;");
  puts("char b = 0b00000011;");

  c = a & b;
  printf("a & b: %d\n", c);

  c = (b >> 2) & 0b00000001;
  printf("(b >> 2) & 0b00000001: %d\n", c);

  c = (b >> 1) & 0b00000001;
  printf("(b >> 1) & 0b00000001: %d\n", c);

  return 0;
}
