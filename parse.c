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

Var *new_var(Var *next, char *name, int len, int offset) {
    Var *var = calloc(1, sizeof(Var));
    var->next = next;
    var->name =name;
    var->len = len;
    var->offset = offset;
    return var;
}

static VarList *var_list;

Var *find_lvar(Token *tok) {
    VarList *vl;
    Var *var;
    for (vl = var_list; vl; vl = vl->next) {
        Var *var;
        for (var = vl->var; var; var = var->next) {
            if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
                return var;
        }
    }
    return NULL;
}

Function *function();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

Function *program() {
    Function head = {};
    Function *cur = &head;

    while (!at_eof()) {
        cur->next = function();
        cur = cur->next;
    }
    return head.next;
}

Function *function() {
    Function *func = calloc(1, sizeof(Function));
    func->name = expect_ident();
    expect('(');
    func->var_list = calloc(1, sizeof(VarList));
    if (!consume(")")) {
        char *name = expect_ident();
        func->var_list->var = new_var(NULL, name, strlen(name), 8);
        Var *var = func->var_list->var;
        while (consume(",")) {
            name = expect_ident();
            var->next = new_var(NULL, name, strlen(name), var->offset + 8);
            var = var->next;
        }
        expect(')');
    }
    var_list = func->var_list;
    var_list->next = calloc(1, sizeof(VarList));
    var_list->next->var = calloc(1, sizeof(Var));
    expect('{');
    func->code = new_node(ND_BLOCK, NULL, NULL);
    Node *node = func->code;
    while (!consume("}")) {
        if (at_eof()) {
            fprintf(stderr, "}が存在しません");
            exit(1);
        }
        node->next = stmt();
        node = node->next;
    }
    return func;
}

Node *stmt() {
    Node *node;
    if (consume("{")) {
        Node *block = new_node(ND_BLOCK, NULL, NULL);
        node = block;
        while (!consume("}")) {
            if (at_eof()) {
                fprintf(stderr, "}が存在しません");
                exit(1);
            }
            node->next = stmt();
            node = node->next;
        }
        return block;
    } else if (consume("return")) {
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
        if (consume("(")) {
            node->kind = ND_CALL_FUNC;
            node->funcname = strndup(tok->str, tok->len);
            if (!consume(")")) { 
                Node *args = assign();
                node->next = args;
                while (consume(",")) {
                    args->next = assign();
                    args = args->next;
                }
                expect(')');
            }
        } else {
            node->kind = ND_LVAR;
        }

        Var *var = find_lvar(tok);
        if (!var) {
            var = new_var(var_list->next->var, tok->str, tok->len, var_list->next->var->offset + 8);
            node->offset = var->offset;
            var_list->next->var = var;
        }

        if (node->kind == ND_LVAR)
            node->offset = var->offset;

        return node;
    }

    return new_node_num(expect_number());
}

