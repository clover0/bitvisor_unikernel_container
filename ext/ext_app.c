#include "bv_calls.h"

int _start(int a1, int a2)
{
    int i = 1000000;
    for (;;)
    {
        if (i <= 0)
        {
            printf("x,");
            i = 1000000;
        }
        else
        {
            i--;
        }
        bv_yield();
    }
    return 0;
}

// #include <stdio.h>

// int main() {
//     int i = 1000000;
//     for (;;){
//         if (i <= 0)
//         {
//             printf("x,");
//             i = 1000000;
//         } else
//         {
//             i--;
//         }

//     }
//     return 0;
// }