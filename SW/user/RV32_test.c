
int arr[16] = {5,3,5,6,8,5,3,2,5,7,8};
char c_arr[15] = {'a'};
const int size = 16; 

struct some
{
    int mem1;
    char mem2;
    int mem3;
}obj;

int ui_int;
char ui_c;
char ui_c2;
int ui_int2;
char ui_c3;

void populate(int start)
{
    for(int i = 0; i < size; i++)
    {
        arr[i] = start;
        start++;
    }
}

int calc_sum(int* sum)
{
    *sum = 0;
    for(int i = 0 ; i < size; i++)
    {
        *sum += arr[i];
    }
}

int global_int = 10;
char global_char = 'a';
int main()
{
    
    int sum;
    populate(25);
    calc_sum(&sum);   

    return sum;
}