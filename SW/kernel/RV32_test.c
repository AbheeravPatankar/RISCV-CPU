

//Kernel Developement begins

int someFunction(int a , int b)
{
    if( a % 2 == 0)
    {
        return a + b;
    }
    else
    {
        return a - b;
    }
}


int main()
{
    int a = 25;
    int b = 34;
    volatile int res = someFunction(a,b);
    if(res % 4 == 0)
        return 0;
    else
        return 1;
}
   