#include "nagicc.h"

Token *token;
char *user_input;

void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

void error_at(char *loc, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, "");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

bool consume(char op) {
    if (token->kind != TK_OPERATOR || token->str[0] != op) {
        return false;
    }
    token = token->next;
    return true;
}

bool consume_token(TokenKind kind) {
    if (token->kind != kind) {
        return false;
    }
    token = token->next;
    return true;
}

void expect(char op) {
    if (token->kind != TK_OPERATOR || token->str[0] != op) {
        error("'%c'ではありません", op);
    }
    token = token->next;
}

int expect_number() {
    if (token->kind != TK_NUM) {
        error_at(token->str, "数ではありません");
    }
    int val = token->val;
    token = token->next;
    return val;
}

Token *new_token(TokenKind kind, Token *cur, char *str) {
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    cur->next = tok;
    return tok;
}

void tokenize(char *p) {
    Token head;
    head.next = NULL;
    Token *cur = &head;
    user_input = p;

    while (*p) {
        if (isspace(*p)) {
            p++;
        }

        if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')') {
            cur = new_token(TK_OPERATOR, cur, p++);
            continue;
        }

        if (*p == '=') {
            p++;
            if (*p == '=') {
                cur = new_token(TK_EQUAL, cur, p++);
                continue;
            }
        }

        if (*p == '!') {
            p++;
            if (*p == '=') {
                cur = new_token(TK_NEQUAL, cur, p++);
                continue;
            }
        }

        if (*p == '<') {
            p++;
            if (*p == '=') {
                cur = new_token(TK_LESSEQ, cur, p++);
                continue;
            } else {
                p--;
                cur = new_token(TK_OPERATOR, cur, p++);
                continue;
            }
        }

        if (*p == '>') {
            p++;
            if (*p == '=') {
                cur = new_token(TK_GREATEQ, cur, p++);
                continue;
            } else {
                p--;
                cur = new_token(TK_OPERATOR, cur, p++);
                continue;
            }
        }

        if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10);
            continue;
        }

        error_at(p, "トークナイズできません");
    }

    new_token(TK_EOF, cur, p);
    token = head.next;
}
