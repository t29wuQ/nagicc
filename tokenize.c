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

bool consume_token(TokenKind kind) {
    if (token->kind != kind) {
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

bool ahead_semicolon() {
    if (token->kind != TK_RESERVED || token->str[0] != ';') {
        return false;
    }
    return true;
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

char *expect_ident() {
    if (token->kind != TK_IDENT) {
        error("expected an identifer");
    }
    char *name = malloc(sizeof(char) * token->len);
    memcpy(name, token->str, token->len);
    token = token->next;
    return name;
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

int is_alnum(char c) {
    return ('a' <= c  && c <= 'z') ||
        ('A' <= c && c <= 'Z') ||
        c == '_' ||
        ('0' <= c && c <= '9');
}

static char *keywords[] = {"return", "if", "else", "while", "for"}; 
char *is_reserved_keyword(char *c) {
    int i;
    // search keyward
    for (i = 0; i < sizeof(keywords) / sizeof(*keywords); ++i) {
        int len = strlen(keywords[i]);
        if (strncmp(c, keywords[i], len) == 0 && !is_alnum(c[len])) {
            return keywords[i];
        }
    }

    return NULL;
}

static char *datatypes[] = {"int"};
char *is_reserved_datatype(char *c) {
    int i;
    // search datatype
    for (i = 0; i < sizeof(datatypes) / sizeof(*datatypes); ++i) {
        int len = strlen(datatypes[i]);
        if (strncmp(c, datatypes[i], len) == 0 && !is_alnum(c[len])) {
            return datatypes[i];
        }
    }

    return NULL;
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

        if (*p == '+' || *p == '-' || *p == '*' || *p == '/' ||
            *p == '(' || *p == ')' ||
            *p == '{' || *p == '}' ||
            *p == ';' || *p == ',' ||
            *p == '&') {
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

        if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p, 0);
            cur->val = strtol(p, &p, 10);
            continue;
        }

        char *keyword = is_reserved_keyword(p);
        if (keyword) {
            int len = strlen(keyword);
            cur = new_token(TK_RESERVED, cur, keyword, len);
            p += len;
            continue;
        }

        char *datatype = is_reserved_datatype(p);
        if (datatype) {
            int len = strlen(datatype);
            cur = new_token(TK_DATATYPE, cur, datatype, len);
            p += len;
            continue;
        }

        if (('a' <= *p  && *p <= 'z') ||
            ('A' <= *p && *p <= 'Z') ||
            *p == '_') {
            int count = 1;
            ++p;
            while (is_alnum(*p)) {
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
