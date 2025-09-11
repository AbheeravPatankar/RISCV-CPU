
typedef struct sample
{
    int a;
    int b;
    char c;
}Sample;

int check_if(int param1 , int param2)
{
    if(param1 == 5)
     return -1;

    return param1 + param2;
}

int check_while()
{
    // this function will check how the RV32 compiler interprets the while loops in c language.
}

int check_for()
{
    // this function will check hoq the RV32 compiler interprets the for loops in c language.
}

int check_switch()
{
    // this function will check hoq the RV32 compiler interpreta the switch statements/bloacks in c language.
}

int check_struct()
{
    // this function will check how the RV32 compiler interpets user defined structures in c language.
}

int check_pointers()
{
    // this function will check how does the RV32 compiler interprets pointers in c language.
}

int main()
{
    Sample obj = {2,3,'a'};
    int a = obj.a;
    int b = obj.b;
    int c = a + b;
}

