%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

void yyerror(const char *s);
int  yylex(void);
extern int yylineno;

static Node *root = NULL;

/* ── Type inference structures ── */
#define MAX_PROPS 32
#define MAX_TYPES 16
#define MAX_VARS  64

typedef struct { char key[64]; char type[32]; } Prop;
typedef struct { char name[64]; Prop props[MAX_PROPS]; int nprops; } TypeDef;
typedef struct { char name[64]; char type[64]; char scope[32]; } VarEntry;

static TypeDef  types[MAX_TYPES]; static int ntype = 0;
static VarEntry vars[MAX_VARS];   static int nvar  = 0;

/* forward declarations */
static void   infer_program(Node *n);
static void   emit_program(Node *n, FILE *f);
static char  *infer_expr_type(Node *n);
static void   emit_expr(Node *n, FILE *f);
static void   emit_stmt(Node *n, FILE *f, int ind);
static void   emit_block_stmts(NodeList *l, FILE *f, int ind);
static void   indent_f(FILE *f, int d);

static void var_set(const char *name, const char *type, const char *scope) {
    for (int i = 0; i < nvar; i++)
        if (strcmp(vars[i].name, name) == 0 &&
            strcmp(vars[i].scope, scope) == 0) {
            strncpy(vars[i].type, type, 63); return;
        }
    strncpy(vars[nvar].name,  name,  63);
    strncpy(vars[nvar].type,  type,  63);
    strncpy(vars[nvar].scope, scope, 31);
    nvar++;
}

static const char *var_type_in(const char *name, const char *scope) {
    for (int i = 0; i < nvar; i++)
        if (strcmp(vars[i].name, name) == 0 &&
            strcmp(vars[i].scope, scope) == 0) return vars[i].type;
    return "unknown";
}

static const char *var_type(const char *name) {
    for (int i = 0; i < nvar; i++)
        if (strcmp(vars[i].name, name) == 0) return vars[i].type;
    return "unknown";
}

static TypeDef *type_find(const char *name) {
    for (int i = 0; i < ntype; i++)
        if (strcmp(types[i].name, name) == 0) return &types[i];
    return NULL;
}

static TypeDef *type_new(const char *name) {
    if (type_find(name)) return type_find(name);
    strncpy(types[ntype].name, name, 63);
    types[ntype].nprops = 0;
    return &types[ntype++];
}

static void type_add_prop(TypeDef *t, const char *key, const char *ptype) {
    for (int i = 0; i < t->nprops; i++)
        if (strcmp(t->props[i].key, key) == 0) return;
    strncpy(t->props[t->nprops].key,  key,   63);
    strncpy(t->props[t->nprops].type, ptype, 31);
    t->nprops++;
}

/* infer type of a single property value node */
static const char *prop_val_type(Node *v) {
    if (!v) return "unknown";
    if (v->kind == N_BOOL)   return "boolean";
    if (v->kind == N_STR)    return "string";
    if (v->kind == N_NUM)    return "number";
    if (v->kind == N_NULL)   return "null";
    /* Date.now() → number */
    if (v->kind == N_CALL && v->left && v->left->kind == N_MEMBER) {
        Node *m = v->left;
        /* new Date().toISOString() → string */
        if (m->left && m->left->kind == N_NEW) return "string";
        if (m->left && m->left->kind == N_IDENT &&
            strcmp(m->left->sval,"Date")==0 &&
            m->right && strcmp(m->right->sval,"now")==0)
            return "number";
        /* any .toString(), .toISOString(), etc. → string */
        if (m->right && m->right->kind == N_IDENT) {
            const char *mn = m->right->sval;
            if (strstr(mn,"String")!=NULL || strstr(mn,"string")!=NULL ||
                strstr(mn,"ISO")!=NULL || strstr(mn,"Format")!=NULL)
                return "string";
        }
    }
    /* arithmetic → number */
    if (v->kind == N_BINARY) return "number";
    /* member access: param.prop — look up param type */
    if (v->kind == N_MEMBER && v->left && v->left->kind == N_IDENT) {
        const char *pt = var_type(v->left->sval);
        if (strcmp(pt,"unknown") != 0) return pt;
    }
    if (v->kind == N_IDENT) return var_type(v->sval);
    return "unknown";
}

%}

%union { char *str; struct Node *node; struct NodeList *list; }

%token ASYNC FUNCTION CONST LET IF ELSE RETURN THROW NEW TRUE_LIT FALSE_LIT NULL_LIT
%token ARROW OR AND NOT ASSIGN SEMICOLON COLON COMMA DOT
%token LPAREN RPAREN LBRACE RBRACE LBRACKET RBRACKET
%token <str> IDENTIFIER STRING_LIT NUMBER_LIT EQ NEQ LT GT LE GE ADDOP MULOP ASSIGN_OP

/* Precedence: lowest → highest */
%right ARROW
%right ASSIGN ASSIGN_OP
%left  OR
%left  AND
%left  EQ NEQ
%left  LT GT LE GE
%left  ADDOP
%left  MULOP
%right NOT
%left  DOT LPAREN LBRACKET

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%type <node> program expr assign_expr or_expr and_expr eq_expr rel_expr
%type <node> add_expr mul_expr unary_expr postfix_expr primary_expr
%type <node> arrow_func object_literal array_literal prop
%type <node> stmt expr_stmt var_decl if_stmt throw_stmt return_stmt func_decl block
%type <list> stmt_list arg_list prop_list elem_list param_list

%start program

%%

program
    : stmt_list  { root = make_node(N_PROGRAM); root->list = $1; $$ = root; }
    ;

stmt_list
    : stmt_list stmt  { $$ = nl_append($1, $2); }
    | stmt            { $$ = nl_append(NULL, $1); }
    ;

stmt
    : func_decl   { $$ = $1; }
    | var_decl    { $$ = $1; }
    | if_stmt     { $$ = $1; }
    | throw_stmt  { $$ = $1; }
    | return_stmt { $$ = $1; }
    | expr_stmt   { $$ = $1; }
    | block       { $$ = $1; }
    ;

func_decl
    : ASYNC FUNCTION IDENTIFIER LPAREN param_list RPAREN block
      {
        $$ = make_node(N_FUNC);
        $$->sval  = $3;
        $$->ival  = 1; /* async */
        $$->left  = make_node(N_BLOCK); $$->left->list = $5; /* params as block */
        $$->right = $7;
      }
    | FUNCTION IDENTIFIER LPAREN param_list RPAREN block
      {
        $$ = make_node(N_FUNC);
        $$->sval  = $2;
        $$->ival  = 0;
        $$->left  = make_node(N_BLOCK); $$->left->list = $4;
        $$->right = $6;
      }
    ;

param_list
    : param_list COMMA IDENTIFIER { $$ = nl_append($1, make_leaf(N_IDENT, $3)); free($3); }
    | IDENTIFIER                  { $$ = nl_append(NULL, make_leaf(N_IDENT, $1)); free($1); }
    | /* empty */                 { $$ = NULL; }
    ;

block
    : LBRACE stmt_list RBRACE { $$ = make_node(N_BLOCK); $$->list = $2; }
    | LBRACE RBRACE           { $$ = make_node(N_BLOCK); $$->list = NULL; }
    ;

var_decl
    : CONST IDENTIFIER ASSIGN expr SEMICOLON
      { $$ = make_node(N_VAR_DECL); $$->ival=1; $$->sval=$2; $$->left=$4; }
    | CONST IDENTIFIER ASSIGN expr
      { $$ = make_node(N_VAR_DECL); $$->ival=1; $$->sval=$2; $$->left=$4; }
    | LET IDENTIFIER ASSIGN expr SEMICOLON
      { $$ = make_node(N_VAR_DECL); $$->ival=0; $$->sval=$2; $$->left=$4; }
    | LET IDENTIFIER ASSIGN expr
      { $$ = make_node(N_VAR_DECL); $$->ival=0; $$->sval=$2; $$->left=$4; }
    | LET IDENTIFIER SEMICOLON
      { $$ = make_node(N_VAR_DECL); $$->ival=0; $$->sval=$2; $$->left=NULL; }
    ;

if_stmt
    : IF LPAREN expr RPAREN block ELSE block
      { $$ = make_node(N_IF); $$->left=$3; $$->right=$5; $$->extra=$7; }
    | IF LPAREN expr RPAREN block %prec LOWER_THAN_ELSE
      { $$ = make_node(N_IF); $$->left=$3; $$->right=$5; $$->extra=NULL; }
    ;

throw_stmt
    : THROW expr SEMICOLON { $$ = make_node(N_THROW); $$->left=$2; }
    | THROW expr           { $$ = make_node(N_THROW); $$->left=$2; }
    ;

return_stmt
    : RETURN expr SEMICOLON { $$ = make_node(N_RETURN); $$->left=$2; }
    | RETURN expr           { $$ = make_node(N_RETURN); $$->left=$2; }
    | RETURN SEMICOLON      { $$ = make_node(N_RETURN); $$->left=NULL; }
    ;

expr_stmt
    : assign_expr SEMICOLON { $$ = make_node(N_EXPR_STMT); $$->left=$1; }
    | assign_expr           { $$ = make_node(N_EXPR_STMT); $$->left=$1; }
    ;

/* ── Expressions (precedence low → high) ── */

expr : assign_expr { $$ = $1; } ;

assign_expr
    : postfix_expr ASSIGN_OP assign_expr
      { $$ = make_node(N_ASSIGN); $$->sval=$2; $$->left=$1; $$->right=$3; }
    | postfix_expr ASSIGN assign_expr
      { $$ = make_node(N_ASSIGN); $$->sval=strdup("="); $$->left=$1; $$->right=$3; }
    | or_expr { $$ = $1; }
    ;

or_expr
    : or_expr OR and_expr
      { $$ = make_node(N_BINARY); $$->sval=strdup("||"); $$->left=$1; $$->right=$3; }
    | and_expr { $$ = $1; }
    ;

and_expr
    : and_expr AND eq_expr
      { $$ = make_node(N_BINARY); $$->sval=strdup("&&"); $$->left=$1; $$->right=$3; }
    | eq_expr { $$ = $1; }
    ;

eq_expr
    : eq_expr EQ rel_expr
      { $$ = make_node(N_BINARY); $$->sval=$2; $$->left=$1; $$->right=$3; }
    | eq_expr NEQ rel_expr
      { $$ = make_node(N_BINARY); $$->sval=$2; $$->left=$1; $$->right=$3; }
    | rel_expr { $$ = $1; }
    ;

rel_expr
    : rel_expr LT add_expr
      { $$ = make_node(N_BINARY); $$->sval=$2; $$->left=$1; $$->right=$3; }
    | rel_expr GT add_expr
      { $$ = make_node(N_BINARY); $$->sval=$2; $$->left=$1; $$->right=$3; }
    | rel_expr LE add_expr
      { $$ = make_node(N_BINARY); $$->sval=$2; $$->left=$1; $$->right=$3; }
    | rel_expr GE add_expr
      { $$ = make_node(N_BINARY); $$->sval=$2; $$->left=$1; $$->right=$3; }
    | add_expr { $$ = $1; }
    ;

add_expr
    : add_expr ADDOP mul_expr
      { $$ = make_node(N_BINARY); $$->sval=$2; $$->left=$1; $$->right=$3; }
    | mul_expr { $$ = $1; }
    ;

mul_expr
    : mul_expr MULOP unary_expr
      { $$ = make_node(N_BINARY); $$->sval=$2; $$->left=$1; $$->right=$3; }
    | unary_expr { $$ = $1; }
    ;

unary_expr
    : NOT unary_expr
      { $$ = make_node(N_UNARY); $$->sval=strdup("!"); $$->left=$2; }
    | postfix_expr { $$ = $1; }
    ;

postfix_expr
    : postfix_expr DOT IDENTIFIER LPAREN arg_list RPAREN
      {
        /* method call: obj.method(args) */
        Node *mem = make_node(N_MEMBER);
        mem->left  = $1;
        mem->right = make_leaf(N_IDENT, $3); free($3);
        $$ = make_node(N_CALL);
        $$->left = mem;
        $$->list = $5;
      }
    | postfix_expr DOT IDENTIFIER
      {
        $$ = make_node(N_MEMBER);
        $$->left  = $1;
        $$->right = make_leaf(N_IDENT, $3); free($3);
      }
    | postfix_expr LPAREN arg_list RPAREN
      {
        $$ = make_node(N_CALL);
        $$->left = $1;
        $$->list = $3;
      }
    | primary_expr { $$ = $1; }
    ;

primary_expr
    : IDENTIFIER      { $$ = make_leaf(N_IDENT,  $1); free($1); }
    | STRING_LIT      { $$ = make_leaf(N_STR,    $1); free($1); }
    | NUMBER_LIT      { $$ = make_leaf(N_NUM,    $1); free($1); }
    | TRUE_LIT        { $$ = make_node(N_BOOL); $$->ival=1; $$->sval=strdup("true"); }
    | FALSE_LIT       { $$ = make_node(N_BOOL); $$->ival=0; $$->sval=strdup("false"); }
    | NULL_LIT        { $$ = make_node(N_NULL); $$->sval=strdup("null"); }
    | NEW IDENTIFIER LPAREN arg_list RPAREN
      { $$ = make_node(N_NEW); $$->sval=$2; $$->list=$4; }
    | object_literal  { $$ = $1; }
    | array_literal   { $$ = $1; }
    | arrow_func      { $$ = $1; }
    | LPAREN expr RPAREN { $$ = $2; }
    ;

arrow_func
    : IDENTIFIER ARROW assign_expr %prec ARROW
      { $$ = make_node(N_ARROW); $$->left=make_leaf(N_IDENT,$1); free($1); $$->right=$3; }
    | IDENTIFIER ARROW block
      { $$ = make_node(N_ARROW); $$->left=make_leaf(N_IDENT,$1); free($1); $$->right=$3; }
    | LPAREN param_list RPAREN ARROW block
      { $$ = make_node(N_ARROW);
        $$->left = make_node(N_BLOCK); $$->left->list=$2;
        $$->right=$5; }
    ;

object_literal
    : LBRACE prop_list RBRACE { $$ = make_node(N_OBJECT); $$->list=$2; }
    | LBRACE RBRACE           { $$ = make_node(N_OBJECT); $$->list=NULL; }
    ;

prop_list
    : prop_list COMMA prop { $$ = nl_append($1, $3); }
    | prop                 { $$ = nl_append(NULL, $1); }
    ;

prop
    : IDENTIFIER COLON expr
      { $$ = make_node(N_PROP); $$->sval=$1; $$->left=$3; }
    | STRING_LIT COLON expr
      { $$ = make_node(N_PROP); $$->sval=$1; $$->left=$3; }
    ;

array_literal
    : LBRACKET elem_list RBRACKET { $$ = make_node(N_ARRAY); $$->list=$2; }
    | LBRACKET RBRACKET           { $$ = make_node(N_ARRAY); $$->list=NULL; }
    ;

elem_list
    : elem_list COMMA expr { $$ = nl_append($1, $3); }
    | expr                 { $$ = nl_append(NULL, $1); }
    ;

arg_list
    : arg_list COMMA expr { $$ = nl_append($1, $3); }
    | expr                { $$ = nl_append(NULL, $1); }
    | /* empty */         { $$ = NULL; }
    ;

%%

/* ════════════════════════════════════════════════════════
   SEMANTIC ANALYSIS
   ════════════════════════════════════════════════════════ */

/* PascalCase the first letter of a word */
static void pascal(char *dst, const char *src, int maxlen) {
    strncpy(dst, src, maxlen-1); dst[maxlen-1]='\0';
    if (dst[0]>='a'&&dst[0]<='z') dst[0]-=32;
}

/* Derive a type name from a variable name.
   "newUser" → "User",  "newFile" → "FileRecord" (last capital word),
   "userData" → "UserData" → we strip trailing "Data" and add "Input" */
static char *derive_type_name(const char *vname) {
    static char buf[64];
    /* strip leading "new" */
    const char *p = vname;
    if (strncmp(p,"new",3)==0 && p[3]>='A'&&p[3]<='Z') p+=3;
    pascal(buf, p, 64);
    /* avoid collision with JS/TS built-in types */
    static const char *builtins[] = {
        "File","Error","Event","URL","Map","Set","Date","Array","Object",
        "Promise","Function","Symbol","Number","String","Boolean", NULL
    };
    for (int i=0; builtins[i]; i++)
        if (strcmp(buf, builtins[i])==0) { strncat(buf,"Record",63-strlen(buf)); break; }
    return buf;
}

/* Derive an Input type name from a param name.
   "userData" → "UserInput",  "fileData" → "FileInput",
   "order"    → "OrderInput" */
static char *derive_input_type(const char *pname) {
    static char buf[64];
    /* strip trailing "Data" if present */
    char tmp[64]; strncpy(tmp, pname, 63);
    int len = strlen(tmp);
    if (len>4 && strcmp(tmp+len-4,"Data")==0) tmp[len-4]='\0';
    pascal(buf, tmp, 60);
    strncat(buf, "Input", 63-strlen(buf));
    return buf;
}

/* Walk an object literal and register its shape as a TypeDef.
   For member-access values (param.field), we record type as "string" by default
   since we can't resolve them at this point — they'll be overridden if we see
   a string literal at the call site. */
static void register_object_type(const char *tname, Node *obj) {
    if (!obj || obj->kind != N_OBJECT) return;
    TypeDef *td = type_new(tname);
    for (NodeList *l = obj->list; l; l = l->next) {
        Node *p = l->node;
        if (!p || p->kind != N_PROP) continue;
        const char *pt = prop_val_type(p->left);
        /* member access like userData.email — infer from field name */
        if (strcmp(pt,"unknown")==0 && p->left && p->left->kind==N_MEMBER) {
            Node *field = p->left->right;
            if (field && field->kind==N_IDENT) {
                /* common field name heuristics */
                const char *fn = field->sval;
                if (strstr(fn,"At")!=NULL || strstr(fn,"Date")!=NULL ||
                    strstr(fn,"Name")!=NULL || strstr(fn,"name")!=NULL ||
                    strstr(fn,"email")!=NULL || strstr(fn,"password")!=NULL ||
                    strstr(fn,"status")!=NULL || strstr(fn,"url")!=NULL)
                    pt = "string";
                else if (strstr(fn,"id")!=NULL || strstr(fn,"Id")!=NULL ||
                         strstr(fn,"size")!=NULL || strstr(fn,"count")!=NULL ||
                         strstr(fn,"quantity")!=NULL || strstr(fn,"stock")!=NULL)
                    pt = "number";
                else
                    pt = "string"; /* safe default */
            }
        }
        type_add_prop(td, p->sval, pt);
    }
}

/* Infer return type: find `const newX = { ... }` in body → Promise<X> */
static char *infer_return_type(Node *func) {
    static char buf[64];
    Node *body = func->right;
    if (!body) return "void";
    char last_obj_var[64] = "";
    for (NodeList *l = body->list; l; l = l->next) {
        Node *s = l->node;
        if (s && s->kind == N_VAR_DECL && s->left && s->left->kind == N_OBJECT)
            strncpy(last_obj_var, s->sval, 63);
    }
    if (last_obj_var[0]) {
        snprintf(buf, 63, "Promise<%s>", derive_type_name(last_obj_var));
        return buf;
    }
    return "Promise<unknown>";
}

/* Scan function body and infer param types from usage */
static void infer_param_types(Node *func) {
    NodeList *params = func->left ? func->left->list : NULL;
    Node     *body   = func->right;
    if (!params || !body) return;

    char pnames[8][64]; int np = 0;
    for (NodeList *l = params; l && np < 8; l = l->next)
        if (l->node) strncpy(pnames[np++], l->node->sval, 63);

    /* track which params are arrays */
    int is_array[8] = {0};

    for (NodeList *sl = body->list; sl; sl = sl->next) {
        Node *s = sl->node;
        if (!s) continue;

        /* expr_stmt: param.push/find/filter/map → array */
        if (s->kind == N_EXPR_STMT && s->left) {
            Node *e = s->left;
            if (e->kind == N_CALL && e->left && e->left->kind == N_MEMBER) {
                Node *obj = e->left->left, *mth = e->left->right;
                if (obj && obj->kind == N_IDENT && mth)
                    for (int i=0;i<np;i++)
                        if (strcmp(obj->sval,pnames[i])==0 &&
                            (strcmp(mth->sval,"push")==0||strcmp(mth->sval,"find")==0||
                             strcmp(mth->sval,"filter")==0||strcmp(mth->sval,"map")==0))
                            is_array[i]=1;
            }
        }

        /* const x = param.find/filter → param is array */
        if (s->kind == N_VAR_DECL && s->left) {
            Node *e = s->left;
            if (e->kind == N_CALL && e->left && e->left->kind == N_MEMBER) {
                Node *obj = e->left->left, *mth = e->left->right;
                if (obj && obj->kind == N_IDENT && mth)
                    for (int i=0;i<np;i++)
                        if (strcmp(obj->sval,pnames[i])==0 &&
                            (strcmp(mth->sval,"find")==0||strcmp(mth->sval,"filter")==0)) {
                            is_array[i]=1;
                        }
            }
            /* const newX = { ... } → register object type */
            if (s->left->kind == N_OBJECT) {
                char *tn = derive_type_name(s->sval);
                register_object_type(tn, s->left);
                var_set(s->sval, tn, func->sval);
            }
        }
    }

    /* Now assign types to params.
       Array params get ElemType[] where ElemType = the object type found in body.
       Non-array params get InputType derived from param name. */
    char elem_type[64] = "";
    /* find the object type registered for this function */
    for (int i=0;i<nvar;i++)
        if (strcmp(vars[i].scope,func->sval)==0 &&
            strncmp(vars[i].type,"__",2)!=0 &&
            strcmp(vars[i].type,"unknown")!=0 &&
            strcmp(vars[i].type,"string")!=0 &&
            strcmp(vars[i].type,"number")!=0 &&
            strcmp(vars[i].type,"boolean")!=0) {
            strncpy(elem_type, vars[i].type, 63); break;
        }

    for (int i=0;i<np;i++) {
        if (is_array[i]) {
            char arr[72];
            /* Derive element type from param name: "products" → "Product",
               "users" → "User", "files" → "FileRecord" (builtin check) */
            char elem[64];
            /* strip trailing 's' for plurals */
            strncpy(elem, pnames[i], 63);
            int elen = strlen(elem);
            if (elen > 1 && elem[elen-1]=='s') elem[elen-1]='\0';
            pascal(elem, elem, 64);
            /* builtin collision check */
            static const char *builtins2[] = {
                "File","Error","Event","URL","Map","Set","Date","Array","Object",
                "Promise","Function","Symbol","Number","String","Boolean", NULL
            };
            for (int j=0; builtins2[j]; j++)
                if (strcmp(elem, builtins2[j])==0) { strncat(elem,"Record",63-strlen(elem)); break; }
            snprintf(arr,71,"%s[]",elem);
            var_set(pnames[i], arr, func->sval);
        } else if (strcmp(var_type_in(pnames[i],func->sval),"unknown")==0 ||
                   var_type_in(pnames[i],func->sval)[0]=='\0') {
            var_set(pnames[i], derive_input_type(pnames[i]), func->sval);
        }
    }
}

/* Main semantic pass */
static void infer_program(Node *n) {
    if (!n) return;
    for (NodeList *l = n->list; l; l = l->next) {
        Node *s = l->node;
        if (!s) continue;
        if (s->kind == N_FUNC) {
            infer_param_types(s);
            char sig[128], ret[64];
            strncpy(ret, infer_return_type(s), 63);
            snprintf(sig, 127, "async function(...): %s", ret);
            var_set(s->sval, sig, "global");
        }
        if (s->kind == N_VAR_DECL && s->left && s->left->kind == N_ARRAY) {
            var_set(s->sval, "__let_array__", "global");
            /* if array has object literal elements, register their type */
            if (s->left->list && s->left->list->node &&
                s->left->list->node->kind == N_OBJECT) {
                /* derive type name from var name (strip 's') */
                char elem[64]; strncpy(elem, s->sval, 63);
                int elen = strlen(elem);
                if (elen>1 && elem[elen-1]=='s') elem[elen-1]='\0';
                pascal(elem, elem, 64);
                register_object_type(elem, s->left->list->node);
            }
        }
    }
}

/* ════════════════════════════════════════════════════════
   SYMBOL TABLE PRINT
   ════════════════════════════════════════════════════════ */
static void print_symtable(void) {
    printf("\n╔══════════════════════════════════════════════════════════════╗\n");
    printf("║              SYMBOL TABLE (Semantic Analysis)                ║\n");
    printf("╠════════════════╦══════════════════════════════╦══════════════╣\n");
    printf("║ %-14s ║ %-28s ║ %-12s ║\n", "Identifier", "Type", "Scope");
    printf("╠════════════════╬══════════════════════════════╬══════════════╣\n");
    for (int i = 0; i < nvar; i++)
        printf("║ %-14s ║ %-28s ║ %-12s ║\n",
               vars[i].name, vars[i].type, vars[i].scope);
    printf("╚════════════════╩══════════════════════════════╩══════════════╝\n");
}

/* ════════════════════════════════════════════════════════
   CODE GENERATION
   ════════════════════════════════════════════════════════ */

static void indent_f(FILE *f, int d) {
    for (int i = 0; i < d; i++) fprintf(f, "    ");
}

/* Resolve the TypeScript type for a param variable */
static const char *ts_param_type(const char *pname, const char *fname) {
    return var_type_in(pname, fname);
}

/* Emit a TypeScript type for a variable name used in an expression */
static const char *ts_type_of(const char *name) {
    return var_type(name);
}

static void emit_expr(Node *n, FILE *f) {
    if (!n) return;
    switch (n->kind) {
    case N_IDENT:  fprintf(f, "%s", n->sval); break;
    case N_STR:    fprintf(f, "%s", n->sval); break;
    case N_NUM:    fprintf(f, "%s", n->sval); break;
    case N_BOOL:   fprintf(f, "%s", n->sval); break;
    case N_NULL:   fprintf(f, "null"); break;
    case N_UNARY:
        fprintf(f, "%s", n->sval);
        emit_expr(n->left, f);
        break;
    case N_BINARY:
        emit_expr(n->left, f);
        fprintf(f, " %s ", n->sval);
        emit_expr(n->right, f);
        break;
    case N_ASSIGN:
        emit_expr(n->left, f);
        fprintf(f, " %s ", n->sval);
        emit_expr(n->right, f);
        break;
    case N_MEMBER:
        emit_expr(n->left, f);
        fprintf(f, ".");
        emit_expr(n->right, f);
        break;
    case N_CALL: {
        /* special: array.find(arrow) — annotate arrow param with element type */
        int is_find = 0;
        const char *elem_type = NULL;
        if (n->left && n->left->kind == N_MEMBER) {
            Node *mth = n->left->right;
            Node *obj = n->left->left;
            if (mth && (strcmp(mth->sval,"find")==0 ||
                        strcmp(mth->sval,"filter")==0 ||
                        strcmp(mth->sval,"map")==0)) {
                is_find = 1;
                if (obj && obj->kind == N_IDENT) {
                    const char *at = ts_type_of(obj->sval);
                    /* strip [] */
                    static char et[64];
                    strncpy(et, at, 63);
                    int len = strlen(et);
                    if (len > 2 && et[len-2]=='[' && et[len-1]==']')
                        et[len-2] = '\0';
                    elem_type = et;
                }
            }
        }
        emit_expr(n->left, f);
        fprintf(f, "(");
        int first = 1;
        for (NodeList *l = n->list; l; l = l->next) {
            if (!first) fprintf(f, ", ");
            first = 0;
            Node *arg = l->node;
            /* annotate arrow param if this is a find/filter/map call */
            if (is_find && arg && arg->kind == N_ARROW &&
                arg->left && arg->left->kind == N_IDENT && elem_type) {
                fprintf(f, "(%s: %s) => ", arg->left->sval, elem_type);
                emit_expr(arg->right, f);
            } else {
                emit_expr(arg, f);
            }
        }
        fprintf(f, ")");
        break;
    }
    case N_NEW:
        fprintf(f, "new %s(", n->sval);
        { int first=1;
          for (NodeList *l=n->list;l;l=l->next){
              if(!first)fprintf(f,", ");first=0;
              emit_expr(l->node,f);
          }
        }
        fprintf(f, ")");
        break;
    case N_ARROW:
        if (n->left && n->left->kind == N_IDENT)
            fprintf(f, "%s => ", n->left->sval);
        emit_expr(n->right, f);
        break;
    case N_OBJECT: {
        fprintf(f, "{\n");
        int first = 1;
        for (NodeList *l = n->list; l; l = l->next) {
            Node *p = l->node;
            if (!p || p->kind != N_PROP) continue;
            if (!first) fprintf(f, ",\n");
            first = 0;
            fprintf(f, "        %s: ", p->sval);
            emit_expr(p->left, f);
        }
        fprintf(f, "\n    }");
        break;
    }
    case N_ARRAY:
        fprintf(f, "[");
        { int first=1;
          for (NodeList *l=n->list;l;l=l->next){
              if(!first)fprintf(f,", ");first=0;
              emit_expr(l->node,f);
          }
        }
        fprintf(f, "]");
        break;
    default: break;
    }
}

static void emit_stmt(Node *n, FILE *f, int ind) {
    if (!n) return;
    switch (n->kind) {
    case N_EXPR_STMT:
        indent_f(f, ind);
        emit_expr(n->left, f);
        fprintf(f, ";\n");
        break;
    case N_VAR_DECL: {
        indent_f(f, ind);
        const char *kw = n->ival ? "const" : "let";
        /* determine TS type annotation */
        char ts_type[128] = "";
        if (n->left) {
            if (n->left->kind == N_OBJECT) {
                char *tn = derive_type_name(n->sval);
                snprintf(ts_type, 127, ": %s", tn);
            } else if (n->left->kind == N_ARRAY) {
                /* let x = [] — look up inferred type */
                const char *vt = var_type(n->sval);
                if (strcmp(vt,"__let_array__")==0 || strcmp(vt,"unknown")==0) {
                    /* try to find element type from function context */
                    snprintf(ts_type, 127, ": unknown[]");
                } else {
                    snprintf(ts_type, 127, ": %s", vt);
                }
            } else if (n->left->kind == N_NUM ||
                       (n->left->kind == N_BINARY)) {
                snprintf(ts_type, 127, ": number");
            }
        }
        fprintf(f, "%s %s%s", kw, n->sval, ts_type);
        if (n->left) {
            fprintf(f, " = ");
            emit_expr(n->left, f);
        }
        fprintf(f, ";\n");
        break;
    }
    case N_IF:
        indent_f(f, ind);
        fprintf(f, "if (");
        emit_expr(n->left, f);
        fprintf(f, ") {\n");
        emit_block_stmts(n->right ? n->right->list : NULL, f, ind+1);
        indent_f(f, ind);
        fprintf(f, "}");
        if (n->extra) {
            fprintf(f, " else {\n");
            emit_block_stmts(n->extra->list, f, ind+1);
            indent_f(f, ind);
            fprintf(f, "}");
        }
        fprintf(f, "\n");
        break;
    case N_THROW:
        indent_f(f, ind);
        fprintf(f, "throw ");
        emit_expr(n->left, f);
        fprintf(f, ";\n");
        break;
    case N_RETURN:
        indent_f(f, ind);
        fprintf(f, "return");
        if (n->left) { fprintf(f, " "); emit_expr(n->left, f); }
        fprintf(f, ";\n");
        break;
    case N_BLOCK:
        emit_block_stmts(n->list, f, ind);
        break;
    default: break;
    }
}

static void emit_block_stmts(NodeList *l, FILE *f, int ind) {
    for (; l; l = l->next) emit_stmt(l->node, f, ind);
}

/* Emit type alias definitions discovered during semantic analysis */
static void emit_type_aliases(FILE *f) {
    for (int i = 0; i < ntype; i++) {
        TypeDef *td = &types[i];
        fprintf(f, "type %s = {\n", td->name);
        for (int j = 0; j < td->nprops; j++)
            fprintf(f, "    %s: %s;\n", td->props[j].key, td->props[j].type);
        fprintf(f, "};\n\n");
    }
}

/* Emit a function with typed params and return type */
static void emit_func(Node *n, FILE *f) {
    if (!n || n->kind != N_FUNC) return;

    /* collect param names */
    NodeList *params = n->left ? n->left->list : NULL;

    /* infer return type */
    char ret_type[64];
    strncpy(ret_type, infer_return_type(n), 63);

    if (n->ival) fprintf(f, "async ");
    fprintf(f, "function %s(\n", n->sval);

    for (NodeList *l = params; l; l = l->next) {
        Node *p = l->node;
        if (!p) continue;
        const char *pt = ts_param_type(p->sval, n->sval);
        fprintf(f, "    %s: %s", p->sval, pt);
        if (l->next) fprintf(f, ",");
        fprintf(f, "\n");
    }

    if (n->ival)
        fprintf(f, "): %s {\n", ret_type);
    else
        fprintf(f, ") {\n");

    /* emit body */
    if (n->right) emit_block_stmts(n->right->list, f, 1);

    fprintf(f, "}\n\n");
}

/* Emit a top-level let declaration with proper array type */
static void emit_top_let(Node *n, FILE *f) {
    if (!n || n->kind != N_VAR_DECL) return;
    /* find any param typed as X[] across all function scopes */
    char arr_type[64] = "unknown[]";
    for (int i = 0; i < nvar; i++) {
        int len = strlen(vars[i].type);
        if (len > 2 && vars[i].type[len-2]=='[' && vars[i].type[len-1]==']' &&
            strcmp(vars[i].scope,"global")!=0) {
            strncpy(arr_type, vars[i].type, 63);
            break;
        }
    }
    fprintf(f, "let %s: %s = ", n->sval, arr_type);
    if (n->left) emit_expr(n->left, f);
    else fprintf(f, "[]");
    fprintf(f, ";\n");
}

static void emit_program(Node *n, FILE *f) {
    if (!n) return;
    emit_type_aliases(f);
    for (NodeList *l = n->list; l; l = l->next) {
        Node *s = l->node;
        if (!s) continue;
        if (s->kind == N_FUNC) {
            emit_func(s, f);
        } else if (s->kind == N_VAR_DECL && !s->ival) {
            /* top-level let */
            emit_top_let(s, f);
        } else if (s->kind == N_EXPR_STMT) {
            /* top-level call like registerUser(...).then(...) — skip or emit */
            /* skip .then/.catch chains at top level — they're test harness code */
        }
    }
}

/* ════════════════════════════════════════════════════════
   MAIN
   ════════════════════════════════════════════════════════ */

void yyerror(const char *s) {
    fprintf(stderr, "[Parse Error] line %d: %s\n", yylineno, s);
}

int main(void) {
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║         JS → TS Compiler  (Lex + Bison)              ║\n");
    printf("║   Phases: Lexical | Syntax | Semantic | CodeGen      ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    printf("── PHASE 1 & 2: Lexical + Syntax Analysis ─────────────\n");
    if (yyparse() != 0) {
        fprintf(stderr, "Parsing failed.\n");
        return 1;
    }
    printf("  ✔ Parse successful.\n");

    printf("\n── PHASE 3: Semantic Analysis ─────────────────────────\n");
    infer_program(root);
    print_symtable();

    printf("\n── PHASE 4: Code Generation ───────────────────────────\n");
    FILE *out = fopen("output.ts", "w");
    if (!out) { perror("output.ts"); return 1; }
    emit_program(root, out);
    fclose(out);
    printf("  ✔ TypeScript written to output.ts\n");

    ast_free(root);
    printf("\n✅ Compilation successful.\n");
    return 0;
}
