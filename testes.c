#include <stdio.h>
#include <stdlib.h>

int t = 0;

int main(int argc, char **argv)
{
    if (argc != 2)
        return 1;
    t = atoi(argv[1]);

    int result = ((t<<1)^((t<<1)+(t>>7)&t>>12))|t>>(4-(1^7&(t>>19)))|t>>7;
    printf("Result is %d\n", result);

    // ((t<<1)^((t<<1)+(t>>7)&t>>12))|t>>(4-(1^7&(t>>19)))|t>>7
    //
}
