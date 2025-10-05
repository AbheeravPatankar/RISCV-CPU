

//Kernel Developement begins


void writei(int val, int* addr)
{
    // Store 'val' at memory address 'addr'
    *addr = val;
}


int main()
{
    int res = g_arr_test();
    writei(res,3072);
}