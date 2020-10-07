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

