#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

Node *make_node(NodeKind k) {
    Node *n = calloc(1, sizeof(Node));
    n->kind = k;
    return n;
}

Node *make_leaf(NodeKind k, const char *s) {
    Node *n = make_node(k);
    n->sval = s ? strdup(s) : NULL;
    return n;
}

NodeList *nl_append(NodeList *l, Node *n) {
    NodeList *item = calloc(1, sizeof(NodeList));
    item->node = n;
    if (!l) return item;
    NodeList *p = l;
    while (p->next) p = p->next;
    p->next = item;
    return l;
}

void ast_free(Node *n) {
    if (!n) return;
    free(n->sval);
    ast_free(n->left);
    ast_free(n->right);
    ast_free(n->extra);
    NodeList *l = n->list;
    while (l) { NodeList *nx = l->next; ast_free(l->node); free(l); l = nx; }
    free(n);
}
