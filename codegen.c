#include "nagicc.h"

int label = 0;

void gen_lval(Node *node) {
    if (node->kind != ND_LVAR) {
        error("代入の左辺値が変数ではありません");
    }
    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", node->offset);
    printf("  push rax\n");
}

void gen(Node *node) {
    switch (node->kind) {
    case ND_BLOCK:
        node = node->next;
        while (node) {
            gen(node);
            node = node->next;
        }
        return;
    case ND_IF:
        gen(node->cond);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        int l = label++;
        if (node->els)
            printf("  je .L.else.%d\n", l);
        else
            printf("  je .L.end.%d\n", l);
        gen(node->then);
        printf("  jmp .L.end.%d\n", l);
        if (node->els) {
            printf(".L.else.%d:\n", l);
            gen(node->els);
        }
        printf(".L.end.%d:\n", l);
        return;
    case ND_WHILE:
        l = label++;
        printf(".L.begin.%d:\n", l);
        gen(node->cond);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je .L.end.%d\n", l);
        gen(node->then);
        printf("  jmp .L.begin.%d\n", l);
        printf(".L.end.%d:\n", l);
        return;
    case ND_FOR:
        gen(node->lhs);
        l = label++;
        printf(".L.begin.%d:\n", l);
        gen(node->cond);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je .L.end.%d\n", l);
        gen(node->then);
        gen(node->rhs);
        printf("  jmp .L.begin.%d\n", l);
        printf(".L.end.%d:\n", l);
        return;
    case ND_RETURN:
        if (node->rhs)
            gen(node->rhs);
        printf("  pop rax\n");
        printf("  jmp .L.return\n");
        return;
    case ND_NUM:
        printf("  push %d\n", node->val);
        return;
    case ND_LVAR:
        gen_lval(node);
        printf("  pop rax\n");
        printf("  mov rax, [rax]\n");
        printf("  push rax\n");
        return;
    case ND_ASSIGN: 
        gen_lval(node->lhs);
        gen(node->rhs);
        printf("  pop rdi\n");
        printf("  pop rax\n");
        printf("  mov [rax], rdi\n");
        printf("  push rdi\n");
        return;
    default:
        break;
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
    default:
        break;
    }

    printf("  push rax\n");
}
