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

bool consume(char *op) {
    if (token->kind != TK_RESERVED ||
        strlen(op) != token->len ||
        memcmp(token->str, op, token->len)) {
        return false;
    }
    token = token->next;
    return true;
}

Token *consume_ident() {
    if (token->kind != TK_IDENT) {
        return NULL;
    }
    Token *tok = token;
    token = token->next;
    return tok;
}

void expect(char op) {
    if (token->kind != TK_RESERVED || token->str[0] != op) {
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

bool at_eof() {
    return token->kind == TK_EOF;
}

Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
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
            continue;
        }

        if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')') {
            cur = new_token(TK_RESERVED, cur, p++, 1);
            continue;
        }

        if (*p == '=') {
            p++;
            if (*p == '=') {
                p--;
                cur = new_token(TK_RESERVED, cur, p, 2);
                p += 2;
                continue;
            } else {
                p--;
                cur = new_token(TK_RESERVED, cur, p++, 1);
                continue;
            }
        }

        if (*p == '!') {
            p++;
            if (*p == '=') {
                p--;
                cur = new_token(TK_RESERVED, cur, p, 2);
                p += 2;
                continue;
            }
        }

        if (*p == '<') {
            p++;
            if (*p == '=') {
                p--;
                cur = new_token(TK_RESERVED, cur, p, 2);
                p += 2;
                continue;
            } else {
                p--;
                cur = new_token(TK_RESERVED, cur, p++, 1);
                continue;
            }
        }

        if (*p == '>') {
            p++;
            if (*p == '=') {
                p--;
                cur = new_token(TK_RESERVED, cur, p, 2);
                p += 2;
                continue;
            } else {
                p--;
                cur = new_token(TK_RESERVED, cur, p++, 1);
                continue;
            }
        }

        if (*p == ';') {
            cur = new_token(TK_RESERVED, cur, p++, 1);
            continue;
        }

        if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p, 0);
            cur->val = strtol(p, &p, 10);
            continue;
        }

        if (('a' <= *p  && *p <= 'z') ||
            ('A' <= *p && *p <= 'Z') ||
            *p == '_') {
            int count = 1;
            ++p;
            while (('a' <= *p  && *p <= 'z') ||
                    ('A' <= *p && *p <= 'Z') ||
                    *p == '_' ||
                    ('0' <= *p && *p <= '9')) {
                ++count;
                ++p;
            }
            cur = new_token(TK_IDENT, cur, p - count, count);
            continue;
        }

        error_at(p, "トークナイズできません");
    }

    new_token(TK_EOF, cur, p, 0);
    token = head.next;
}