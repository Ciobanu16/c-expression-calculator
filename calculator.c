#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>   // pentru isdigit, isspace
#include <string.h>

//stiva de operatori
typedef struct {
    char data[100];
    int top;
} CharStack;

void initCharStack(CharStack *s) {
    s->top = -1;
}

int isEmptyCharStack(CharStack *s) {
    return s->top == -1;
}

void pushChar(CharStack *s, char c) {
    s->data[++(s->top)] = c;
}

char popChar(CharStack *s) {
    if (isEmptyCharStack(s)) return '\0';
    return s->data[(s->top)--];
}

char peekChar(CharStack *s) {
    if (isEmptyCharStack(s)) return '\0';
    return s->data[s->top];
}

//stiva de valori
typedef struct {
    double data[100];
    int top;
} DoubleStack;

void initDoubleStack(DoubleStack *s) {
    s->top = -1;
}

int isEmptyDoubleStack(DoubleStack *s) {
    return s->top == -1;
}

void pushDouble(DoubleStack *s, double x) {
    s->data[++(s->top)] = x;
}

double popDouble(DoubleStack *s) {
    if (isEmptyDoubleStack(s)) return 0.0;
    return s->data[(s->top)--];
}

//functii utile
// prioritatea operatorilor
int precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    return 0;
}

// verificăm dacă e operator
int isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/';
}

//infix->postfix
// infix: expresia inițială (string)
// postfix: rezultatul (string, cu spații între elemente)
void infixToPostfix(const char *infix, char *postfix) {
    CharStack stack;
    initCharStack(&stack);

    int i = 0;           // index în infix
    int k = 0;           // index în postfix (unde scriem)

    while (infix[i] != '\0') {
        char c = infix[i];

        if (isspace((unsigned char)c)) {
            // ignorăm spațiile
            i++;
            continue;
        }

        if (isdigit((unsigned char)c)) {
            // dacă e cifră, citim tot numărul (poate fi cu mai multe cifre)
            while (isdigit((unsigned char)infix[i])) {
                postfix[k++] = infix[i++];
            }
            postfix[k++] = ' '; // separator între numere / operatori
            continue;
        }

        if (c == '(') {
            pushChar(&stack, c);
        } else if (c == ')') {
            // scoatem până la '('
            while (!isEmptyCharStack(&stack) && peekChar(&stack) != '(') {
                postfix[k++] = popChar(&stack);
                postfix[k++] = ' ';
            }
            // scoatem '('
            if (!isEmptyCharStack(&stack) && peekChar(&stack) == '(') {
                popChar(&stack);
            }
        } else if (isOperator(c)) {
            // cât timp în vârful stivei avem operator cu prioritate >=
            while (!isEmptyCharStack(&stack) &&
                   isOperator(peekChar(&stack)) &&
                   precedence(peekChar(&stack)) >= precedence(c)) {
                postfix[k++] = popChar(&stack);
                postfix[k++] = ' ';
            }
            // punem operatorul curent
            pushChar(&stack, c);
        } else {
            printf("Caracter necunoscut: %c\n", c);
            exit(1);
        }

        i++;
    }

    // golim stiva
    while (!isEmptyCharStack(&stack)) {
        char op = popChar(&stack);
        if (op != '(' && op != ')') {
            postfix[k++] = op;
            postfix[k++] = ' ';
        }
    }

    postfix[k] = '\0';
}

//evaluare postfix
double evaluatePostfix(const char *postfix) {
    DoubleStack stack;
    initDoubleStack(&stack);

    int i = 0;
    int len = strlen(postfix);

    while (i < len) {
        if (isspace((unsigned char)postfix[i])) {
            i++;
            continue;
        }

        if (isdigit((unsigned char)postfix[i])) {
            double val = 0;
            while (i < len && isdigit((unsigned char)postfix[i])) {
                val = val * 10 + (postfix[i] - '0');
                i++;
            }
            pushDouble(&stack, val);
        } else if (isOperator(postfix[i])) {
            double b = popDouble(&stack);
            double a = popDouble(&stack);
            char op = postfix[i];

            double res = 0;
            switch (op) {
                case '+': res = a + b; break;
                case '-': res = a - b; break;
                case '*': res = a * b; break;
                case '/':
                    if (b == 0) {
                        printf("Eroare: impartire la zero!\n");
                        exit(1);
                    }
                    res = a / b; break;
            }
            pushDouble(&stack, res);
            i++;
        } else {
            printf("Token necunoscut in postfix: %c\n", postfix[i]);
            exit(1);
        }
    }

    // rezultatul final este singurul element din stivă
    return popDouble(&stack);
}


int main(void) {
    char infix[256];
    char postfix[512];

    printf("Introdu o expresie (ex: (3 + 4) * 2 - 5):\n");
    if (!fgets(infix, sizeof(infix), stdin)) {
        printf("Eroare la citire.\n");
        return 1;
    }

    infixToPostfix(infix, postfix);
    printf("Postfix: %s\n", postfix);

    double result = evaluatePostfix(postfix);
    printf("Result: %.2f\n", result);

    return 0;
}
