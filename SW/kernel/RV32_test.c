
char foo()
{
    int a = 10;
    int b = 20;
    char c = 'a';
    if( a % 5 == 0)
    {
        c = a * b;
    }
    else
    {
        c = b / a;
    }
    return c ;
}


int main()
{
    int res = foo();
    return res;
}