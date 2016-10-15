#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#include "stack.h"

enum direction {
    LEFT,
    RIGHT
};

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

// order must match the enum above
char *ops[] = {"<<", ">>", "^", "|", "~", "&", "+", "-", "*", "/", "%", "(", ")"};

enum token_type {
    NUMBER,
    OPERATOR,
};

typedef struct token {
    int type;
    int val;
} token;


bool isnum(char *ch);
bool isvalid_char(char *ch);
bool isvalid_pattern(char *pattern);
int precedence(int op);
int associativity(int op); // left or right
int parse_bytebeat(char *pattern);

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
        // printf("%c (%d)\n", pattern[i], pattern[i]);
        if (pattern[i] == 0 || pattern[i] == 32) // EOF or space
            continue;
        if (!isvalid_char(&pattern[i]))
            return false;
    }

    return true;
}

// http://en.cppreference.com/w/c/language/operator_precedence
int precedence(int op)
{
    switch(op) {
    case 0:  // left and right shift
    case 1:
        return 4;
    case 2: // XOR
        return 2;
    case 3: // bitwize OR
        return 1;
    case 4: // bitwize NOT ~
        return 7;
    case 5: // butwize AND
        return 3;
    case 6: // PLUS
    case 7: // MINUS
        return 5;
    case 8: // MULTIPLY
    case 9: // DIVIDE
    case 10: // MODULO
        return 6;
    default:
        return 99;
    }
}

int associativity(int op)
{
    switch(op) {
    case 4:
        return RIGHT;
    default:
        return LEFT;
    }
}


int parse_bytebeat(char *pattern)
{

    if (!isvalid_pattern(pattern)) {
        printf("Beat it ya val jerk - acceptables chars are 0-9,"
                "<, >, |, ^, ~, (,), +, -, /, *, &,\n");
        return EXIT_FAILURE;
    }

    Stack *numberz = calloc(1, sizeof(List));
    stack_init(numberz, NULL);

    Stack *operatorz = calloc(1, sizeof(List));
    stack_init(operatorz, NULL);

    int pattern_len = strlen(pattern);
    for (int i = 0; i < pattern_len; i++) {

        char *ch = &pattern[i];

        // ignore space or EOF
        if (*ch == 32 || *ch == 0)
            continue;

        token *toke = calloc(1, sizeof(token));

        if ( isnum(ch)) {
            int pushy_num = atoi(ch);
            toke->type = NUMBER;
            toke->val = pushy_num;

            stack_push(numberz, (void **)(toke));

            int num_len = (int) log10(pushy_num) +1;
            i += num_len;
        }
        else {
            toke->type = OPERATOR;
            switch (*ch) {
            case(60):
                toke->val = LEFTSHIFT;
                i++;
                break;
            case(62):
                toke->val = RIGHTSHIFT;
                i++;
                break;
            case(94):
                toke->val = XOR;
                break;
            case(124):
                toke->val = OR;
                break;
            case(126):
                toke->val = NOT;
                break;
            case(38):
                toke->val = AND;
                break;
            case(43):
                toke->val = PLUS;
                break;
            case(45):
                toke->val = MINUS;
                break;
            case(42):
                toke->val = MULTIPLY;
                break;
            case(47):
                toke->val = DIVIDE;
                break;
            case(37):
                toke->val = MODULO;
                break;
            case(40):
                toke->val = LEFTBRACKET;
                break;
            case(41):
                toke->val = RIGHTBRACKET;
                break;
            default:
                printf("OOFT!\n");
                return 1;
            }
            printf("OPS! %s\n", ops[toke->val]);
            printf("OPS SIZE! %d\n", stack_size(operatorz));
            // TODO - here - do the right thing
            // if (stack_size(operatorz) > 0) {
            //     bool wurk_it = true;
            //     while(wurk_it) {
            //         // check precedence
            //         token *top_o_the_stack = stack_peek(operatorz);
            //         if ((associativity(toke->val) == LEFT && precedence(toke->val) <= precedence(top_o_the_stack->val)) ||
            //            (associativity(toke->val) == RIGHT && precedence(toke->val) < precedence(top_o_the_stack->val))) {
            //                if (stack_pop(operatorz, (void **) &top_o_the_stack) == 0) {
            //                    stack_push(numberz, (void **)top_o_the_stack);
            //                } else printf("couldnae pop, capn\n");
            //         } else {
            //             wurk_it = false;
            //         }
            //     }
            // }
            stack_push(operatorz, (void **)(toke));
        }
    }

    token *re_token;

    printf("NUMBERZ SIZE %d\n", stack_size(numberz));
    while (stack_pop(numberz, (void **) &re_token) == 0) {
      printf("Num %d\n", re_token->val);
    }

    printf("\nOPERATORZ SIZE %d\n", stack_size(operatorz));
    while (stack_pop(operatorz, (void **) &re_token) == 0) {
      printf("OP %s\n", ops[re_token->val]);
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
