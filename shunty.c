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
int  parse_bytebeat(char *pattern);

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
    static char acceptable[] = {'<', '>', '|', '^', '~', '+',
                                '-', '/', '*', '&', '%'};

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
                "<, >, |, ^, ~, +, -, /, *, &, %%\n");
        return EXIT_FAILURE;
    }

    Stack *outputz = calloc(1, sizeof(List));
    stack_init(outputz, NULL);

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
        printf("ATOI result is %d\n", atoi(wurd));
        int val = atoi(wurd);
        if (val != 0) {
            printf("NUM! %d\n", val);
            toke->type = NUMBER;
            toke->val = val;
            stack_push(outputz, (void **)(toke));
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
             else {
                 printf("OOFT!\n");
                 return 1;
             }
             if (stack_size(operatorz) > 0) {
                 printf("STACK SIZE GRETR THAN 0: %d\n", stack_size(operatorz));
                 bool wurk_it = true;
                 while(wurk_it && stack_size(operatorz) > 0) {
                     token *top_o_the_stack = stack_peek(operatorz);
                     printf("PRECEDENCE OF ME %d, precedence of top of stack %d\n", precedence(toke->val), precedence(top_o_the_stack->val));
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
                        //    printf("GIN IN\n");
                        if (stack_pop(operatorz, (void **) &top_o_the_stack) == 0) {
                            stack_push(outputz, (void **) top_o_the_stack);
                        } else {
                            printf("Couldnae pop, mate\n");
                        }
                     } else {
                            wurk_it = false;
                            printf("WURKIT False\n");
                     }
                 }
             }
             stack_push(operatorz, (void **)(toke));
        }
    }

    printf("\n\n======================================\n");
    token *re_token;

    printf("\nPOOOP\n");
    while (stack_pop(operatorz, (void **) &re_token) == 0) {
      printf("POP TYPE %d\n", re_token->type);
      printf("POP val %d\n", re_token->val);
      stack_push(outputz, (void**) re_token);
    }

    printf("\nOUTPUTZ SIZE %d\n", stack_size(outputz));
    while (stack_pop(outputz, (void **) &re_token) == 0) {
      if ( re_token->type == NUMBER)
        printf("Num %d\n", re_token->val);
      else if ( re_token->type == OPERATOR)
        printf("Op %s\n", ops[re_token->val]);
      else
        printf("Wowo, nelly!\n");
    }

    printf("\nOOPS SIZE %d\n", stack_size(operatorz));
    while (stack_pop(operatorz, (void **) &re_token) == 0) {
      if ( re_token->type == NUMBER)
        printf("Num %d\n", re_token->val);
      else if ( re_token->type == OPERATOR)
        printf("Op %s\n", ops[re_token->val]);
      else
        printf("Wowo, nelly!\n");
    }

    stack_destroy(operatorz);
    stack_destroy(outputz);

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
