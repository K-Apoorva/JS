#ifndef AST_H
#define AST_H

/* ── node kinds ── */
typedef enum {
    /* expressions */
    N_IDENT, N_STR, N_NUM, N_BOOL, N_NULL,
    N_MEMBER,       /* obj.prop          */
    N_CALL,         /* callee(args)      */
    N_NEW,          /* new Ctor(args)    */
    N_ARROW,        /* param => body     */
    N_OBJECT,       /* { props }         */
    N_ARRAY,        /* [ elems ]         */
    N_UNARY,        /* !x                */
    N_BINARY,       /* x OP y            */
    N_ASSIGN,       /* x OP= y           */
    N_CHAIN,        /* expr.method(args) — chained call */
    /* statements */
    N_EXPR_STMT,
    N_VAR_DECL,     /* const/let x = e   */
    N_IF,           /* if(c){t}{e}       */
    N_THROW,
    N_RETURN,
    N_FUNC,         /* async? function name(params){body} */
    N_BLOCK,        /* { stmts }         */
    N_PROP,         /* key: value        */
    N_PROGRAM,
} NodeKind;

typedef struct Node Node;

/* linked list of nodes */
typedef struct NodeList {
    Node           *node;
    struct NodeList *next;
} NodeList;

struct Node {
    NodeKind kind;
    char    *sval;   /* identifier name / string value / operator */
    int      ival;   /* 1=const,0=let for VAR_DECL; 1=async for FUNC; bool value */
    Node    *left;   /* condition / object / callee / param / lhs */
    Node    *right;  /* then-branch / body / rhs / value */
    Node    *extra;  /* else-branch / arrow-body */
    NodeList *list;  /* args / params / props / stmts / elems */
};

Node     *make_node(NodeKind k);
Node     *make_leaf(NodeKind k, const char *s);
NodeList *nl_append(NodeList *l, Node *n);
void      ast_free(Node *n);

#endif
