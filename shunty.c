#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#include "stack.h"

#define INT2VOIDP(i) (void*)(uintptr_t)(i)

enum ops {
    LEFTSHIFT,
    RIGHTSHIFT,
    XOR,
    OR,
    NOT,
    AND,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    MODULO,
    LEFTBRACKET,
    RIGHTBRACKET
};

bool isnum(char *ch);
bool isvalid_char(char *ch);
bool isvalid_pattern(char *pattern);


//////////////////////////////////////////////////////////////////////////////

bool isnum(char *ch)
{
    if ( *ch >= 48 && *ch <= 57)
        return true;

    return false;
}

bool isvalid_char(char *ch)
{
    if (isnum(ch))
        return true;
    
    // else check for valid chars in a bitwise operation
    static char acceptable[] = {'<', '>', '|', '^', '~', '(', ')',
                                '+', '-', '/', '*', '&', '%'};

    int acceptable_len = strlen(acceptable);
    for ( int i = 0; i < acceptable_len; i++) {
        if ( *ch == acceptable[i] )
            return true;
    }
    return false;
}

bool isvalid_pattern(char *pattern)
{
    int pattern_len = strlen(pattern);

    for (int i = 0; i < pattern_len; i++) {
        printf("%c (%d)\n", pattern[i], pattern[i]);
        if (pattern[i] == 0 || pattern[i] == 32) // EOF or space
            continue;
        if (!isvalid_char(&pattern[i]))
            return false;
    }

    return true;
}

int parse_bytebeat(char *pattern);
int parse_bytebeat(char *pattern)
{
    Stack *numberz = calloc(1, sizeof(List));
    stack_init(numberz, NULL);

    Stack *operatorz = calloc(1, sizeof(List));
    stack_init(operatorz, NULL);

    if (!isvalid_pattern(pattern)) {
        printf("Beat it ya val jerk - acceptables chars are 0-9,"
                "<, >, |, ^, ~, (,), +, -, /, *, &,\n");
        return EXIT_FAILURE;
    }

    int pattern_len = strlen(pattern);
    for (int i = 0; i < pattern_len; i++) {

        char *ch = &pattern[i];

        // ignore space or EOF
        if (*ch == 32 || *ch == 0)
            continue;

        if ( isnum(ch)) {
            int pushy_num = atoi(ch);
            int num_len = (int) log10(pushy_num) +1;
            stack_push(numberz, INT2VOIDP(pushy_num));
            i += num_len;
        }
        else {
            switch (*ch) {
            case(60):
                stack_push(operatorz, INT2VOIDP(LEFTSHIFT));
                i++;
                break;
            case(62):
                stack_push(operatorz, INT2VOIDP(RIGHTSHIFT));
                i++;
                break;
            case(94):
                stack_push(operatorz, INT2VOIDP(XOR));
                break;
            case(124):
                stack_push(operatorz, INT2VOIDP(OR));
                break;
            case(126):
                stack_push(operatorz, INT2VOIDP(NOT));
                break;
            case(38):
                stack_push(operatorz, INT2VOIDP(AND));
                break;
            case(43):
                stack_push(operatorz, INT2VOIDP(PLUS));
                break;
            case(45):
                stack_push(operatorz, INT2VOIDP(MINUS));
                break;
            case(42):
                stack_push(operatorz, INT2VOIDP(MULTIPLY));
                break;
            case(47):
                stack_push(operatorz, INT2VOIDP(DIVIDE));
                break;
            case(37):
                stack_push(operatorz, INT2VOIDP(MODULO));
                break;
            case(40):
                stack_push(operatorz, INT2VOIDP(LEFTBRACKET));
                break;
            case(41):
                stack_push(operatorz, INT2VOIDP(RIGHTBRACKET));
                break;
            default:
                printf("OOFT!\n");
                return 1;
            }
        }
    }

    printf("NUMBERZ SIZE %d\n", stack_size(numberz));
    int c;
    while (stack_pop(numberz, (void **) &c) == 0) {
      printf("Num %d\n", c);
    }

    printf("\nOPERATORZ SIZE %d\n", stack_size(operatorz));
    int op;
    while (stack_pop(operatorz, (void **) &op) == 0) {
      printf("OP %d\n", op);
    }

    stack_destroy(operatorz);
    stack_destroy(numberz);

    return EXIT_SUCCESS;
}


int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("Dingie mate - gimme a quote delimited bytebeat\n");
        return EXIT_FAILURE;
    }

    char pattern[128];
    return parse_bytebeat(strncpy(pattern, argv[1], 127));

}
