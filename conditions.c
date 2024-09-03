#include <stdio.h>
#include <stdbool.h>

int main(int argc, char const *argv[])
{
  bool isDev = true;

  if (isDev)
  {
    puts("I'm a Dev!!!");
  }

  short a = 1234;
  short b;

  if (a < 1337)
  {
    b = 0;
  }
  else
  {
    b = 1746;
  }

  return 0;
}
