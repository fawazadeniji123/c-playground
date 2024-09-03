#include <stdio.h>
#include <stdint.h>

struct Person
{
  char name[64];
  int age;
};

int main(int argc, char const *argv[])
{
  printf("sizeof person struct %zu bytes\n", sizeof(uint128_t));
  return 0;
}
