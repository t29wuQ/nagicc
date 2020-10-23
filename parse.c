#include "nagicc.h"

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

LVar *find_lvar(Token *tok) {
    LVar *var;
    for (var = locals; var; var = var->next) {
        if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
            return var;
    }
    return NULL;
}

Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

void program() {
    int i = 0;
    while (!at_eof()) {
        code[i++] = stmt();
    }
    code[i] = NULL;
}

Node *stmt() {
    Node *node;
    if (consume("return")) {
        if (ahead_semicolon()) {
            expect(';');
            node = new_node(ND_RETURN, NULL, NULL);
            return node;
        }
        node = new_node(ND_RETURN, NULL, expr());
    } else if (consume("if")) {
        node = calloc(1, sizeof(Node));
        node->kind = ND_IF;
        expect('(');
        node->cond = expr();
        expect(')');
        node->then = stmt();
        if (consume("else")) {
            node->els = stmt();
        }
        return node;
    } else if(consume("while")) {
        node = calloc(1, sizeof(Node));
        node->kind = ND_WHILE;
        expect('(');
        node->cond = expr();
        expect(')');
        node->then = stmt();
        return node;
    } else if (consume("for")) {
        node = calloc(1, sizeof(Node));
        node->kind = ND_FOR;
        expect('(');
        node->lhs = expr();
        expect(';');
        node->cond = expr();
        expect(';');
        node->rhs = expr();
        expect(')');
        node->then = stmt();
        return node;
    } else {
        node = expr();
    }
    expect(';');
    return node;
}

Node *expr() {
    return assign();
}

Node *assign() {
    Node *node = equality();
    if (consume("=")) {
        node = new_node(ND_ASSIGN, node, assign());
    }
    return node;
}

Node *equality() {
    Node *node = relational();

    for (;;) {
        if (consume("==")) {
            node = new_node(ND_EQUAL, node, relational());
        }
        else if (consume("!="))
            node = new_node(ND_NEQUAL, node, relational());
        else
            return node;
    }
}

Node *relational() {
    Node *node = add();
    
    for (;;) {
        if (consume("<"))
            node = new_node(ND_LESS, node, add());
        else if (consume(">"))
            node = new_node(ND_LESS, add(), node);
        else if (consume("<="))
            node = new_node(ND_LESSEQ, node, add());
        else if (consume(">="))
            node = new_node(ND_LESSEQ, add(), node);
        else
            return node;
    }
}

Node *add() {
    Node *node = mul();

    for (;;) {
        if (consume("+"))
            node = new_node(ND_ADD, node, mul());
        else if (consume("-"))
            node = new_node(ND_SUB, node, mul());
        else
            return node;
    }
}

Node *mul() {
    Node *node = unary();

    for (;;) {
        if (consume("*")) 
            node = new_node(ND_MUL, node, unary());
        else if(consume("/"))
            node = new_node(ND_DIV, node, unary());
        else
            return node;
    }
}

Node *unary() {
    if (consume("+"))
        return primary();
    else if (consume("-"))
        return new_node(ND_SUB, new_node_num(0), primary());
    else
        return primary();
}

Node *primary() {
    if (consume("(")) {
        Node *node = expr();
        expect(')');
        return node;
    }

    Token *tok = consume_ident();
    if (tok) {
        Node *node = calloc(1, sizeof(Node));
        node->kind = ND_LVAR;

        LVar *lvar = find_lvar(tok);
        if (lvar) {
            node->offset = lvar->offset;
        } else {
            lvar = calloc(1, sizeof(LVar));
            lvar->next = locals;
            lvar->name = tok->str;
            lvar->len = tok->len;
            lvar->offset = locals->offset + 8;
            node->offset = lvar->offset;
            locals = lvar;
        }
        return node;
    }

    return new_node_num(expect_number());
}

