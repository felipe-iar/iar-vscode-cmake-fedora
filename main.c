#include <intrinsics.h>

__root char table[5];

int foo(int a, int b)
{
  return a + b;
}

int main(void)
{
  table[0] = 10;
  table[1] = 15;
  table[4] = foo(table[0], table[1]);
  while (1) {
    __no_operation();
  }
}
