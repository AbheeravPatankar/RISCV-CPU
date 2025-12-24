#pragma once

#define KMEM_START 2147483648   // PA =  0x80000000     VA = 0x0000000
#define KMEM_SIZE  20480     // 20 KB

void init_kernel_paging();