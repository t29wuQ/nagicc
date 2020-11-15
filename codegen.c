#include "nagicc.h"

static int label = 1;
static char *func_name;
static char *argsreg[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

void gen(Node *node);

void gen_lval(Node *node) {
    if (node->kind != ND_LVAR) {
        error("代入の左辺値が変数ではありません");
    }
    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", node->offset);
    printf("  push rax\n");
}

void load(void) {
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
}

void store(void) {
    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  mov [rax], rdi\n");
    printf("  push rdi\n");
}

void gen_addr(Node *node) {
    switch (node->kind) {
    case ND_LVAR:
        printf("  lea rax, [rbp-%d]\n", node->offset);
        printf("  push rax\n");
        return;
    case ND_DEREF:
        gen(node->lhs);
        return;
    default:
        error("代入の左辺値が変数ではありません");
    }
}

void gen(Node *node) {
    int l = 0;
    int count = 0;
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
        l = label++;
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
        printf("  jmp .L.return.%s\n", func_name);
        return;
    case ND_NUM:
        printf("  push %d\n", node->val);
        return;
    case ND_LVAR:
        gen_addr(node);
        load();
        return;
    case ND_ASSIGN: 
        gen_addr(node->lhs);
        gen(node->rhs);
        store();
        return;
    case ND_ADDR:
        gen_addr(node->lhs);
        return;
    case ND_DEREF:
        gen(node->lhs);
        load();
        return;
    case ND_CALL_FUNC:
        count = 0;
        while (node->next) {
            gen(node->next);
            printf("  pop %s\n", argsreg[count++]);
            node->next = node->next->next;
        }
        l = label++;
        printf("  mov rax, rsp\n");
        printf("  and rax, 15\n");
        printf("  jnz .L.call.%d\n", l);
        printf("  mov rax, 0\n");
        printf("  call %s\n", node->funcname);
        printf("  jmp .L.end.%d\n", l);
        printf(".L.call.%d:\n", l);
        printf("  sub rsp, 8\n");
        printf("  mov rax, 0\n");
        printf("  call %s\n", node->funcname);
        printf("  add rsp, 8\n");
        printf(".L.end.%d:\n", l);
        printf("  push rax\n");
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

void gen_func(Function *func) {
    printf("%s:\n", func->name);
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    Var *args;
    int args_count = 0;
    for (args = func->var_list->var; args; args = args->next) {
        printf("  push %s\n", argsreg[args_count++]);
    }
    if (func->var_list->next->var)
        printf("  sub rsp, %d\n", func->var_list->next->var->offset);
    func_name = func->name;
    gen(func->code);
    printf(".L.return.%s:\n", func_name);
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
}
