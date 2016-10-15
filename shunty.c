#include <stdio.h>
#include <string.h>

#include "stack.h"

#define INT2VOIDP(i) (void*)(uintptr_t)(i)

int t = 0;

int parse_bytebeat(char *pattern);
int parse_bytebeat(char *pattern)
{
    Stack *elementz = calloc(1, sizeof(List));
    stack_init(elementz, NULL);

    char *c = pattern;
    // doing this once here as the increment in the while loop eats the first val before use.
    // if i just put the increment in the body of the loop, vim complains about unused var
    stack_push(elementz, INT2VOIDP(*c));
    while (*c++) {
        printf("Charrrr %c\n", *c);
        stack_push(elementz, INT2VOIDP(*c));
    }

    int ch;
    printf("SIZE %d\n", stack_size(elementz));
    while (stack_pop(elementz, (void **) &ch) == 0) {
      printf("Woof! %c\n", (char)ch);
    }


    return 0;
}


int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("Dingie mate - gimme a quote delimited bytebeat\n");
        return 1;
    }

    char pattern[128];
    parse_bytebeat(strncpy(pattern, argv[1], 127));

}
