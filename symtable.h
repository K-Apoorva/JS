#ifndef SYMTABLE_H
#define SYMTABLE_H

void        sym_add(const char *name, const char *type, const char *scope);
const char *sym_type(const char *name);
void        print_symtable(void);

#endif
