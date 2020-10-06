#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdarg.h>
#include<string.h>
#include<ctype.h>

typedef enum {
    TK_OPERATOR,
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
} NodeKind;

typedef struct Node Node;

struct Node {
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val;
};

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

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs =rhs;
    return node;
}

Node *new_node_num(int val) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
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

bool at_eof() {
    return token->kind == TK_EOF;
}

Token *new_token(TokenKind kind, Token *cur, char *str) {
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    cur->next = tok;
    return tok;
}

Token *tokenize(char *p) {
    Token head;
    head.next = NULL;
    Token *cur = &head;

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
    return head.next;
}

Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

Node *expr() {
    return equality();
}

Node *equality() {
    Node *node = relational();

    for (;;) {
        if (consume_token(TK_EQUAL))
            node = new_node(ND_EQUAL, node, relational());
        else if (consume_token(TK_NEQUAL))
            node = new_node(ND_NEQUAL, node, relational());
        else
            return node;
    }
}

Node *relational() {
    Node *node = add();
    
    for (;;) {
        if (consume('<'))
            node = new_node(ND_LESS, node, add());
        else if (consume('>'))
            node = new_node(ND_LESS, add(), node);
        else if (consume_token(TK_LESSEQ))
            node = new_node(ND_LESSEQ, node, add());
        else if (consume_token(TK_GREATEQ))
            node = new_node(ND_LESSEQ, add(), node);
        else
            return node;
    }
}

Node *add() {
    Node *node = mul();

    for (;;) {
        if (consume('+'))
            node = new_node(ND_ADD, node, mul());
        else if (consume('-'))
            node = new_node(ND_SUB, node, mul());
        else
            return node;
    }
}

Node *mul() {
    Node *node = unary();

    for (;;) {
        if (consume('*')) 
            node = new_node(ND_MUL, node, unary());
        else if(consume('/'))
            node = new_node(ND_DIV, node, unary());
        else
            return node;
    }
}

Node *unary() {
    if (consume('+'))
        return primary();
    else if (consume('-'))
        return new_node(ND_SUB, new_node_num(0), primary());
    else
        return primary();
}

Node *primary() {
    if (consume('(')) {
        Node *node = expr();
        expect(')');
        return node;
    }

    return new_node_num(expect_number());
}

void gen(Node *node) {
    if (node->kind == ND_NUM) {
        printf("  push %d\n", node->val);
        return;
    }
    
    gen(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->kind) {
    case ND_ADD:
        printf("  add rax, rdi\n");
        break;
    case ND_SUB:
        printf("  sub rax, rdi\n");
        break;
    case ND_MUL:
        printf("  imul rax, rdi\n");
        break;
    case ND_DIV:
        printf("  cqo\n");
        printf("  idiv rdi\n");
        break;
    case ND_EQUAL:
        printf("  cmp rax, rdi\n");
        printf("  sete al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_NEQUAL:
        printf("  cmp rax, rdi\n");
        printf("  setne al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_LESS:
        printf("  cmp rax, rdi\n");
        printf("  setl al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_LESSEQ:
        printf("  cmp rax, rdi\n");
        printf("  setle al\n");
        printf("  movzb rax, al\n");
        break;
    }

    printf("  push rax\n");
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "引数の個数が正しくありません");
        return 1;
    }

    user_input = argv[1];
    token = tokenize(argv[1]);
    Node *node = expr();

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    gen(node);

    printf("  pop rax\n");
    printf("  ret\n");
    return 0;
}
