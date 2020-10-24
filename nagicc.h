#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdarg.h>
#include<string.h>
#include<ctype.h>

typedef enum {
    TK_RESERVED,
    TK_IDENT,
    TK_NUM,
    TK_EOF,
} TokenKind;

typedef struct Token Token;
struct Token {
    TokenKind kind;
    Token *next;
    int val;
    char *str;
    int len;
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
    ND_RETURN,
    ND_IF,
    ND_WHILE,
    ND_FOR,
    ND_BLOCK,
    ND_CALL_FUNC,
} NodeKind;

typedef struct Node Node;

struct Node {
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    Node *cond;
    Node *then;
    Node *els;
    Node *next;
    int val;
    int offset;
    char *funcname;
};

Node *code[100];

typedef struct LVar LVar;
struct LVar {
    LVar *next;
    char *name;
    int len;
    int offset;
};

LVar *locals;

void error(char *fmt, ...);
void tokenize(char *p);
bool consume(char *op);
bool consume_token(TokenKind kind);
Token *consume_ident();
void expect(char op);
int expect_number();
bool ahead_semicolon();
bool at_eof();
void program();
void gen(Node *node);

