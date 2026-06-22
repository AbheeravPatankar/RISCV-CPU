#include "libio.h"

int main()
{
    int a = 2;
    int b = 3;
    rv_printf("The value of the 2 num is %d and %d", a, b);
    
    int x1 = 10, x2 = 20, x3 = 30, x4 = 40, x5 = 50;
    int x6 = 60, x7 = 70, x8 = 80, x9 = 90;

    rv_printf("vals: %d %d %d %d %d %d %d %d %d",
          x1, x2, x3, x4, x5, x6, x7, x8, x9);
}