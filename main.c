#include "nagicc.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "引数の個数が正しくありません");
        return 1;
    }

    locals = calloc(1, sizeof(LVar));
    tokenize(argv[1]);
    program();

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    if (locals->offset > 0) {
        printf("  sub rsp, %d\n", locals->offset);
    }

    int i = 0;
    for (i = 0; code[i]; i++) {
        gen(code[i]);
        // printf("  pop rax\n");
    }

    printf(".L.return:\n");
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return 0;
}
