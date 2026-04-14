#include <stdio.h>
#include <string.h>
#include "symtable.h"

#define MAX_SYM 64

typedef struct {
    char name[64];
    char type[64];
    char scope[32];
} Symbol;

static Symbol symtable[MAX_SYM];
static int    sym_count = 0;

void sym_add(const char *name, const char *type, const char *scope) {
    for (int i = 0; i < sym_count; i++)
        if (strcmp(symtable[i].name, name) == 0) {
            strcpy(symtable[i].type, type);
            return;
        }
    strcpy(symtable[sym_count].name,  name);
    strcpy(symtable[sym_count].type,  type);
    strcpy(symtable[sym_count].scope, scope);
    sym_count++;
}

const char *sym_type(const char *name) {
    for (int i = 0; i < sym_count; i++)
        if (strcmp(symtable[i].name, name) == 0)
            return symtable[i].type;
    return "unknown";
}

void print_symtable(void) {
    printf("\n╔══════════════════════════════════════════════════════╗\n");
    printf("║              SYMBOL TABLE (Semantic Analysis)        ║\n");
    printf("╠══════════════╦══════════════════════════╦════════════╣\n");
    printf("║ %-12s ║ %-24s ║ %-10s ║\n", "Identifier", "Type", "Scope");
    printf("╠══════════════╬══════════════════════════╬════════════╣\n");
    for (int i = 0; i < sym_count; i++)
        printf("║ %-12s ║ %-24s ║ %-10s ║\n",
               symtable[i].name, symtable[i].type, symtable[i].scope);
    printf("╚══════════════╩══════════════════════════╩════════════╝\n");
}
