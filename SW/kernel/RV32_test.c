
char foo(char* a, char* b)
{
    if(*a == 'a')
    {
        return a;
    }
    else
    {
        return b;
    }
}


int main()
{
    char a = 'a';
    char b = 'e';
    int res = foo(&a, &b);
    return res;
}