#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdarg.h>
#include<string.h>
#include<ctype.h>

typedef enum {
    TK_OPERATOR,
    TK_IDENT,
    TK_EQUAL, // ==
    TK_NEQUAL, // !=
    TK_LESSEQ, // <=
    TK_GREATEQ, // >=
    TK_NUM,
    TK_EOF,
} TokenKind;

typedef struct Token Token;
struct Token {
    TokenKind kind;
    Token *next;
    int val;
    char *str;
};

typedef enum {
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_LESS,
    ND_EQUAL, // ==
    ND_NEQUAL, // !=
    ND_LESSEQ, // <=
    ND_NUM,
    ND_ASSIGN,
    ND_LVAR,
} NodeKind;

typedef struct Node Node;

struct Node {
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val;
    int offset;
};

Node *code[100];

void error(char *fmt, ...);
void tokenize(char *p);
bool consume(char op);
bool consume_token(TokenKind kind);
Token *consume_ident();
void expect(char op);
int expect_number();
bool at_eof();
void program();
void gen(Node *node);

