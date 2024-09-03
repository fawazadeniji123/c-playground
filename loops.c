#include <stdio.h>

int main(int argc, char const *argv[])
{
  int arr[5];
  char i;

  // initialize all elements of arr to 1337
  while (i < 5)
  {
    arr[i] = 1337;
    i++;
  }

  for (char i = 0; i < 5; i++)
  {
    arr[i] = 1000;
  }

  return 0;
}
