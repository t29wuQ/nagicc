#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdarg.h>
#include<string.h>
#include<ctype.h>

typedef enum {
    TK_RESERVED,
    TK_DATATYPE,
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
    ND_ADDR,
    ND_DEREF,
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

typedef struct Var Var;
struct Var {
    Var *next;
    char *name;
    int len;
    int offset;
};

typedef struct VarList VarList;
struct VarList {
    VarList *next;
    Var *var;
};

typedef struct Function Function;
struct Function {
    Function *next;
    char *name;
    Node *code;
    VarList *var_list;
    int stack_size;
};

void error(char *fmt, ...);
void tokenize(char *p);
bool consume(char *op);
bool consume_token(TokenKind kind);
Token *consume_ident();
void expect(char op);
int expect_number();
char *expect_ident();
bool ahead_semicolon();
bool at_eof();

Function *program();

void gen_func(Function *func);

