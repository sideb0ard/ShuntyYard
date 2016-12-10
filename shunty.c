#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "stack.h"

enum direction { LEFT, RIGHT };

enum unary_or_binary { UNARY, BINARY };

enum token_type {
    NUMBER,
    OPERATOR,
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
    RIGHTBRACKET,
    TEE // letter 't' time counter
};

// order must match the enum above
char *ops[] = {"<<", ">>", "^", "|", "~", "&", "+",
               "-",  "*",  "/", "%", "(", ")", "t"};

typedef struct token {
    int type;
    int val;
} token;

int associativity(int op); // left or right
int bin_or_uni(int op);
char calc(int operator, int first_operand, int val);
int parse_bytebeat(int t, char *pattern, Stack *stack);
int parse_rpn(Stack *rpn_stack);
int precedence(int op);
bool isnum(char *ch);
bool isvalid_char(char *ch);
bool isvalid_pattern(char *pattern);
void reverse_stack(Stack *rpn_stack);

//////////////////////////////////////////////////////////////////////////////

bool isnum(char *ch)
{
    if (*ch >= 48 && *ch <= 57)
        return true;

    return false;
}

bool isvalid_char(char *ch)
{
    if (isnum(ch))
        return true;

    // else check for valid chars in a bitwise operation
    static char acceptable[] = {'<', '>', '|', '^', '~', '+', '-',
                                '/', '*', '&', '%', '(', ')', 't'};

    int acceptable_len = strlen(acceptable);
    for (int i = 0; i < acceptable_len; i++) {
        if (*ch == acceptable[i])
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
    switch (op) {
    case 0: // left and right shift
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
    case 8:  // MULTIPLY
    case 9:  // DIVIDE
    case 10: // MODULO
        return 6;
    case 11: // left and right bracket
    case 12:
        return 0;
    default:
        return 99;
    }
}

int associativity(int op)
{
    switch (op) {
    case 4:
        return RIGHT;
    default:
        return LEFT;
    }
}

int bin_or_uni(int op)
{
    switch (op) {
    case 4: // bitwise NOT ~
        return UNARY;
    default:
        return BINARY;
    }
}

int parse_bytebeat(int t, char *pattern, Stack *rpn_stack)
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
    for (wurd = strtok_r(pattern, sep, &wurdleft); wurd;
         wurd = strtok_r(NULL, sep, &wurdleft)) {
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
            else if (strcmp(wurd, "t") == 0)
                toke->val = TEE;
            else {
                printf("OOFT!\n");
                return 1;
            }
            if (toke->val == TEE) {
                toke->type = NUMBER;
                toke->val = t;
                stack_push(rpn_stack, (void *)(toke));
                continue;
            }
            if (toke->val == LEFTBRACKET) {
                stack_push(operatorz, (void *)(toke));
                continue;
            }
            if (toke->val == RIGHTBRACKET) {
                printf("RIGHT BRACKET!\n");
                while (stack_size(operatorz) > 0) {
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
                while (wurk_it && stack_size(operatorz) > 0) {
                    token *top_o_the_stack = stack_peek(operatorz);
                    printf(
                        "PRECEDENCE OF ME %d, precedence of top of stack %d\n",
                        precedence(toke->val),
                        precedence(top_o_the_stack->val));
                    printf("ASSOCIATIVITY OF ME %s, ASSOCIATIVITY of top of "
                           "stack %s\n",
                           associativity(toke->val) ? "RIGHT" : "LEFT",
                           associativity(top_o_the_stack->val) ? "RIGHT"
                                                               : "LEFT");
                    if (((associativity(toke->val) == LEFT) &&
                         (precedence(toke->val) <=
                          precedence(top_o_the_stack->val))) ||
                        ((associativity(toke->val) == RIGHT) &&
                         (precedence(toke->val) <
                          precedence(top_o_the_stack->val)))) {
                        printf("POPing to PUSH\n");
                        if (stack_pop(operatorz, (void **)&top_o_the_stack) ==
                            0) {
                            stack_push(rpn_stack, (void *)top_o_the_stack);
                        }
                        else {
                            printf("Couldnae pop, mate\n");
                        }
                    }
                    else {
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

    while (stack_pop(operatorz, (void **)&re_token) == 0) {
        stack_push(rpn_stack, (void *)re_token);
    }

    stack_destroy(operatorz);

    return EXIT_SUCCESS;
}

char calc(int operator, int first_operand, int second_operand)
{
    switch (operator) {
    case 0: // left shift
        return first_operand << second_operand;
    case 1: // right shift
        return first_operand >> second_operand;
    case 2: // XOR
        return first_operand ^ second_operand;
    case 3: // bitwize OR
        return first_operand | second_operand;
    case 4: // bitwize NOT ~ // should never hit this as its Unary
        return 99;
    case 5: // bitwize AND
        return first_operand & second_operand;
    case 6: // PLUS
        return first_operand + second_operand;
    case 7: // MINUS
        return first_operand - second_operand;
    case 8: // MULTIPLY
        return first_operand * second_operand;
    case 9: // DIVIDE
        return first_operand / second_operand;
    case 10: // MODULO
        return first_operand % second_operand;
    default:
        return 99;
    }
}

int parse_rpn(Stack *rpn_stack)
{
    printf("\n\n======================================\n");

    Stack *ans_stack = calloc(1, sizeof(List));
    stack_init(ans_stack, NULL);

    printf("SIZEOF PRSEN STACK %d\n", stack_size(rpn_stack));

    token *re_token;
    while (stack_pop(rpn_stack, (void **)&re_token) == 0) {
        if (re_token->type == NUMBER) {
            printf("Num %d\n", re_token->val);
            stack_push(ans_stack, (void *)re_token);
        }
        else if (re_token->type == OPERATOR) {
            printf("Op %s\n", ops[re_token->val]);
            if (bin_or_uni(re_token->val) == UNARY) {
                printf("UNARY!\n");
                token *first_operand;
                stack_pop(ans_stack, (void **)&first_operand);
                ;
                int ans = ~first_operand->val;
                first_operand->val = ans;
                stack_push(ans_stack, (void *)first_operand);
            }
            else {
                printf("BINARY!\n");
                if (stack_size(ans_stack) < 2) {
                    printf("Barf, not enough operands on stack\n");
                    return 1;
                }
                token *second_operand;
                stack_pop(ans_stack, (void **)&second_operand);
                ;
                token *first_operand;
                stack_pop(ans_stack, (void **)&first_operand);
                ;

                printf("Wurking on %d and %d\n", first_operand->val,
                       second_operand->val);
                char ans = calc(re_token->val, first_operand->val,
                               second_operand->val);

                printf("CAlced! %d\n", ans);

                token *ans_toke = calloc(1, sizeof(token));
                ans_toke->type = NUMBER;
                ans_toke->val = ans;
                stack_push(ans_stack, (void *)ans_toke);

                free(second_operand);
                free(first_operand);
            }
        }
        else {
            printf("Wowo, nelly!\n");
        }
        re_token = stack_peek(ans_stack);
        printf("TOTESOFAR: %d\n", re_token->val);
    }

    int ret_val = -1;

    if (stack_size(ans_stack) == 1) {
        token *atoke;
        stack_pop(ans_stack, (void **)&atoke);
        ret_val = atoke->val;
        free(atoke);
        printf("Woop, gots an answer: %d\n", ret_val);
    }
    else {
        printf("Nah man, too many items on stack, sumthing is fucked\n");
    }

    stack_destroy(ans_stack);
    return ret_val;
}

void reverse_stack(Stack *stack)
{
    printf("\n=============\n");
    Stack *rev_stack = calloc(1, sizeof(List));
    stack_init(rev_stack, NULL);

    token *tokey;
    while (stack_size(stack) > 0) {
        stack_pop(stack, (void **)&tokey);
        if (tokey->type == NUMBER)
            printf(":: %d\n", tokey->val);
        else
            printf(":: %s\n", ops[tokey->val]);
        stack_push(rev_stack, (void *)tokey);
    }

    Stack tmp_stack = *stack;
    *stack = *rev_stack;
    stack_destroy(&tmp_stack);
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        printf("Dingie mate - gimme a quote delimited bytebeat\n");
        return EXIT_FAILURE;
    }

    int t = 1;

    if (argc == 3) {
        t = atoi(argv[2]);
    }

    printf("T is %d\n", t);

    char pattern[128];
    strncpy(pattern, argv[1], 127);

    Stack *rpn_stack = calloc(1, sizeof(List));
    stack_init(rpn_stack, NULL);

    parse_bytebeat(t, pattern, rpn_stack);
    reverse_stack(rpn_stack);

    printf("SIZEOF RETRUNED RVRS STACK %d\n", stack_size(rpn_stack));

    int ans = parse_rpn(rpn_stack);
    printf("ANSWER IS %d\n", ans);
    double scale_val = (2.0 / 256 * ans);
    printf("scaled ANSWER IS %f\n", scale_val);

    stack_destroy(rpn_stack);
}
