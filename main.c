#include "nagicc.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "引数の個数が正しくありません");
        return 1;
    }

    tokenize(argv[1]);
    Function *func = program();

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");

    while (func) {
        gen_func(func);
        func = func->next;
    }

    return 0;
}
