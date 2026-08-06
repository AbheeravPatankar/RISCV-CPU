#include "libio.h"

int main()
{
    
    int x1 = 10, x2 = 20, x3 = 30, x4 = 40, x5 = 50;
    int x6 = 60, x7 = 70, x8 = 80, x9 = 90;


    char a = 'a', b = 'b', c = 'c';
    // rv_printf("vals: %d %c %d %c %d %d %d %d %d",
    //       x1, a, x3, b, x5, x6, x7, x8, x9);

    rv_printf("less args: %d %c %d %c %c \n",x1,a,x2,b,c);

    rv_printf("hello world in my C \n");
}