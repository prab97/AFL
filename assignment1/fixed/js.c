#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum {
    TOKEN_NUMBER, TOKEN_PLUS, TOKEN_MINUS, TOKEN_MUL, TOKEN_DIV,
    TOKEN_LPAREN, TOKEN_RPAREN, TOKEN_ID, TOKEN_ASSIGN, TOKEN_SEMI,
    TOKEN_LET, TOKEN_COMMA, TOKEN_DOT, TOKEN_EOF
} TokenType;

typedef struct {
    TokenType type;
    char text[64];
} Token;

typedef struct { char name[64]; double value; } Var;
Var vars[128];
int var_count = 0;

const char *src;
Token current;

// === TOKENIZER ===
void next_token() {
    while (isspace(*src)) src++;

    if (*src == '\0') { current.type = TOKEN_EOF; return; }

    if (isdigit(*src)) {
        int i = 0;
        while (isdigit(*src) || *src == '.') current.text[i++] = *src++;
        current.text[i] = '\0';
        current.type = TOKEN_NUMBER;
        return;
    }

    if (isalpha(*src)) {
        int i = 0;
        while (isalnum(*src)) current.text[i++] = *src++;
        current.text[i] = '\0';
        if (strcmp(current.text, "let") == 0) current.type = TOKEN_LET;
        else current.type = TOKEN_ID;
        return;
    }

    switch (*src) {
        case '+': current.type = TOKEN_PLUS; src++; return;
        case '-': current.type = TOKEN_MINUS; src++; return;
        case '*': current.type = TOKEN_MUL; src++; return;
        case '/': current.type = TOKEN_DIV; src++; return;
        case '(': current.type = TOKEN_LPAREN; src++; return;
        case ')': current.type = TOKEN_RPAREN; src++; return;
        case '=': current.type = TOKEN_ASSIGN; src++; return;
        case ';': current.type = TOKEN_SEMI; src++; return;
        case ',': current.type = TOKEN_COMMA; src++; return;
        case '.': current.type = TOKEN_DOT; src++; return;
    }

    printf("Unexpected char: %c\n", *src);
    exit(1);
}

// === PARSER & INTERPRETER ===
double parse_expr(); // forward

double parse_factor() {
    double val;
    if (current.type == TOKEN_NUMBER) {
        val = atof(current.text);
        next_token();
        return val;
    } else if (current.type == TOKEN_ID) {
        // Handle console.log()
        if (strcmp(current.text, "console") == 0) {
            next_token();
            if (current.type == TOKEN_DOT) {
                next_token();
                if (strcmp(current.text, "log") == 0) {
                    next_token();
                    if (current.type != TOKEN_LPAREN) {
                        printf("Expected '('\n"); exit(1);
                    }
                    next_token();
                    double arg = parse_expr();
                    printf("%g\n", arg);
                    if (current.type != TOKEN_RPAREN) {
                        printf("Expected ')'\n"); exit(1);
                    }
                    next_token();
                    return 0;
                }
            }
        }

        // Handle variable lookup
        for (int i = 0; i < var_count; i++)
            if (strcmp(vars[i].name, current.text) == 0) {
                val = vars[i].value;
                next_token();
                return val;
            }
        printf("Undefined variable: %s\n", current.text);
        exit(1);
    } else if (current.type == TOKEN_LPAREN) {
        next_token();
        val = parse_expr();
        if (current.type != TOKEN_RPAREN) {
            printf("Expected ')'\n");
            exit(1);
        }
        next_token();
        return val;
    } else {
        printf("Unexpected token in factor\n");
        exit(1);
    }
}

double parse_term() {
    double val = parse_factor();
    while (current.type == TOKEN_MUL || current.type == TOKEN_DIV) {
        TokenType op = current.type;
        next_token();
        double right = parse_factor();
        if (op == TOKEN_MUL) val *= right;
        else val /= right;
    }
    return val;
}

double parse_expr() {
    double val = parse_term();
    while (current.type == TOKEN_PLUS || current.type == TOKEN_MINUS) {
        TokenType op = current.type;
        next_token();
        double right = parse_term();
        if (op == TOKEN_PLUS) val += right;
        else val -= right;
    }
    return val;
}

void parse_statement() {
    if (current.type == TOKEN_LET) {
        next_token();
        if (current.type != TOKEN_ID) { printf("Expected identifier\n"); exit(1); }
        char name[64]; strcpy(name, current.text);
        next_token();
        if (current.type != TOKEN_ASSIGN) { printf("Expected '='\n"); exit(1); }
        next_token();
        double val = parse_expr();
        if (current.type == TOKEN_SEMI) next_token();
        vars[var_count++] = (Var){ .value = val };
        strcpy(vars[var_count - 1].name, name);
    } else {
        double val = parse_expr();
        if (current.type == TOKEN_SEMI) next_token();
        // If not console.log, show result
        printf("Result: %g\n", val);
    }
}

void interpret(const char *code) {
    src = code;
    next_token();
    while (current.type != TOKEN_EOF)
        parse_statement();
}

// === MAIN ENTRY POINT ===
char *read_file(const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (!f) { perror("Cannot open file"); exit(1); }
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);
    char *buf = malloc(size + 1);
    fread(buf, 1, size, f);
    buf[size] = '\0';
    fclose(f);
    return buf;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <script.js>\n", argv[0]);
        return 1;
    }
    char *code = read_file(argv[1]);
    interpret(code);
    free(code);
    return 0;
}
