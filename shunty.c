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
int  precedence(int op);
int  associativity(int op); // left or right
int  parse_bytebeat(char *pattern, Stack *stack);
int  parse_rpn(Stack *rpn_stack);
void reverse_stack(Stack *rpn_stack);

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
    static char acceptable[] = {'<', '>', '|', '^', '~', '+', '-',
                                '/', '*', '&', '%', '(', ')'};

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


int parse_bytebeat(char *pattern, Stack *rpn_stack)
{

    if (!isvalid_pattern(pattern)) {
        printf("Beat it ya val jerk - acceptables chars are 0-9,"
                "<, >, |, ^, ~, +, -, /, *, &, %%\n");
        return EXIT_FAILURE;
    }

    Stack *operatorz = calloc(1, sizeof(List));
    stack_init(operatorz, NULL);

    // int pattern_len = strlen(pattern);

    char *sep = " ";
    char *wurd, *wurdleft;
    for (wurd = strtok_r(pattern, sep, &wurdleft);
         wurd;
         wurd = strtok_r(NULL, sep, &wurdleft))
    {
        token *toke = calloc(1, sizeof(token));
        printf("\nToken is %s\n", wurd);
        int val = atoi(wurd);
        if (val != 0) {
            printf("NUM! %d\n", val);
            toke->type = NUMBER;
            toke->val = val;
            stack_push(rpn_stack, (void *)(toke));
        }
        else {
            printf("OP! %s\n", wurd);

             toke->type = OPERATOR;
             if (strcmp(wurd, "<<") == 0)
                 toke->val = LEFTSHIFT;
             else if (strcmp(wurd, ">>") == 0)
                 toke->val = RIGHTSHIFT;
             else if (strcmp(wurd, "^") == 0)
                 toke->val = XOR;
             else if (strcmp(wurd, "|") == 0)
                 toke->val = OR;
             else if (strcmp(wurd, "~") == 0)
                 toke->val = NOT;
             else if (strcmp(wurd, "&") == 0)
                 toke->val = AND;
             else if (strcmp(wurd, "+") == 0)
                 toke->val = PLUS;
             else if (strcmp(wurd, "-") == 0)
                 toke->val = MINUS;
             else if (strcmp(wurd, "*") == 0)
                 toke->val = MULTIPLY;
             else if (strcmp(wurd, "/") == 0)
                 toke->val = DIVIDE;
             else if (strcmp(wurd, "%") == 0)
                 toke->val = MODULO;
             else if (strcmp(wurd, "(") == 0)
                 toke->val = LEFTBRACKET;
             else if (strcmp(wurd, ")") == 0)
                 toke->val = RIGHTBRACKET;
             else {
                 printf("OOFT!\n");
                 return 1;
             }
             if (toke->val == LEFTBRACKET) {
                stack_push(operatorz, (void *)(toke));
                continue;
             }
             if (toke->val == RIGHTBRACKET) {
                printf("RIGHT BRACKET!\n");
                while (stack_size(operatorz) > 0 ) {
                    printf("STACKSIZE %d\n", stack_size(operatorz));
                
                    token *top_o_the_stack = stack_peek(operatorz);

                    printf("Looking at %s\n", ops[top_o_the_stack->val]);

                    if (top_o_the_stack->val == LEFTBRACKET) {
                        printf("WOOP - found bracjet!\n");
                        stack_pop(operatorz, (void **)&top_o_the_stack);
                    }
                    else {
                        stack_pop(operatorz, (void **)&top_o_the_stack);
                        stack_push(rpn_stack, (void *)(top_o_the_stack));
                    }
                }
                continue;
             }

             if (stack_size(operatorz) > 0) {
                 token *top_o_the_stack = stack_peek(operatorz);
                 if (top_o_the_stack->val == LEFTBRACKET) {
                     stack_push(operatorz, (void *)(toke));
                     continue;
                 }
                 printf("STACK SIZE GRETR THAN 0: %d\n", stack_size(operatorz));
                 bool wurk_it = true;
                 while(wurk_it && stack_size(operatorz) > 0) {
                     token *top_o_the_stack = stack_peek(operatorz);
                     printf("PRECEDENCE OF ME %d, precedence of top of stack %d\n",
                             precedence(toke->val), precedence(top_o_the_stack->val));
                     printf("ASSOCIATIVITY OF ME %s, ASSOCIATIVITY of top of stack %s\n",
                             associativity(toke->val) ? "RIGHT" : "LEFT",
                             associativity(top_o_the_stack->val) ? "RIGHT" : "LEFT");
                     if (
                        ((associativity(toke->val) == LEFT) &&
                         (precedence(toke->val) <= precedence(top_o_the_stack->val)))
                        ||
                         ((associativity(toke->val) == RIGHT) && 
                          (precedence(toke->val) < precedence(top_o_the_stack->val)))
                        )
                     {
                         printf("POPing to PUSH\n");
                        if (stack_pop(operatorz, (void **) &top_o_the_stack) == 0) {
                            stack_push(rpn_stack, (void *) top_o_the_stack);
                        } else {
                            printf("Couldnae pop, mate\n");
                        }
                     } else {
                            wurk_it = false;
                            printf("WURKIT False\n");
                     }
                 }
             }
             stack_push(operatorz, (void *)(toke));
        }
        printf("STACK_SIZE(operatorz): %d\n", stack_size(operatorz));
        printf("STACK_SIZE(rpn_stack): %d\n", stack_size(rpn_stack));
    }

    token *re_token;

    while (stack_pop(operatorz, (void **) &re_token) == 0) {
      stack_push(rpn_stack, (void*) re_token);
    }

    stack_destroy(operatorz);

    return EXIT_SUCCESS;
}

int parse_rpn(Stack *rpn_stack)
{
    printf("\n\n======================================\n");

    Stack *ans_stack = calloc(1, sizeof(List));
    stack_init(ans_stack, NULL);

    printf("SIZEOF PRSEN STACK %d\n", stack_size(rpn_stack));

    token *re_token;
    while (stack_pop(rpn_stack, (void **) &re_token) == 0) {
      if ( re_token->type == NUMBER)
        printf("Num %d\n", re_token->val);
      else if ( re_token->type == OPERATOR)
        printf("Op %s\n", ops[re_token->val]);
      else
        printf("Wowo, nelly!\n");
    }

    stack_destroy(ans_stack);
    return EXIT_SUCCESS;
}

void reverse_stack(Stack *stack)
{
    Stack *rev_stack = calloc(1, sizeof(List));
    stack_init(rev_stack, NULL);

    printf("\n===============\n"
           "SIZEOF stack: %d\n", stack_size(stack));
    printf("REVERSEnn!\n");

    token *tokey;
    while (stack_size(stack) > 0)
    {
        stack_pop(stack, (void **) &tokey);
        stack_push(rev_stack, (void *) tokey);
    }

    printf("SIZEOF stack: %d\n", stack_size(stack));
    printf("SIZEOF rev_stack: %d\n", stack_size(rev_stack));

    Stack tmp_stack = *stack;
    *stack = *rev_stack;
    printf("SIZEOF  nstack: %d\n", stack_size(stack));
    stack_destroy(&tmp_stack);
    printf("SIZEOF  nstack: %d\n", stack_size(stack));
}


int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("Dingie mate - gimme a quote delimited bytebeat\n");
        return EXIT_FAILURE;
    }

    char pattern[128];
    strncpy(pattern, argv[1], 127);

    Stack *rpn_stack = calloc(1, sizeof(List));
    stack_init(rpn_stack, NULL);

    parse_bytebeat(pattern, rpn_stack);
    reverse_stack(rpn_stack);

    printf("SIZEOF RETRUNED RVRS STACK %d\n", stack_size(rpn_stack));

    int ans = parse_rpn(rpn_stack);
    printf("ANSWER IS %d\n", ans);

    stack_destroy(rpn_stack);

}
