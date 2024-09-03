#include <stdio.h>

struct point
{
  int x;
  int y;
};

int main(int argc, char const *argv[])
{
  /* code */
  struct point point; //is this good practice? 
  struct point p2 = {2, 3};
  point = (struct point){1, 2};
  p2.x = 4;
  printf("p1.x = %d, p1.y = %d\n", point.x, point.y);
  printf("p2.x = %d, p2.y = %d\n", p2.x, p2.y);
  return 0;
}
