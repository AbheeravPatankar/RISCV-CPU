
int foo(int a, int b)
{
    if( a > b )
    {
        return a - b;
    }
    else
    {
        return b - a;
    }
}


int main()
{
    int a = 10;
    int b = 20;
    int result = foo(a,b);
    return 0;
}