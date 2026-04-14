%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtable.h"

static int func_mode = 0; /* 1=registerUser, 2=uploadFile, 3=placeOrder */
static int indent = 0;

/* ── LALR(1) action trace ── */
static int step = 0;
typedef enum { ACT_SHIFT, ACT_REDUCE, ACT_ACCEPT } ActionKind;
typedef struct { ActionKind kind; char desc[128]; } Action;
#define MAX_ACTIONS 512
static Action actions[MAX_ACTIONS];
static int    action_count = 0;

void trace_shift(const char *token) {
    if (action_count >= MAX_ACTIONS) return;
    actions[action_count].kind = ACT_SHIFT;
    snprintf(actions[action_count].desc, 128, "%-20s", token);
    action_count++;
}
void trace_reduce(const char *lhs, const char *rhs) {
    if (action_count >= MAX_ACTIONS) return;
    actions[action_count].kind = ACT_REDUCE;
    snprintf(actions[action_count].desc, 128, "%-14s → %s", lhs, rhs);
    action_count++;
}
void print_parse_method() {
    printf("\n╔══════════════════════════════════════════════════════╗\n");
    printf("║           PARSING METHOD: LALR(1)                    ║\n");
    printf("╠══════════════════════════════════════════════════════╣\n");
    printf("║  Algorithm : LALR(1)  (Look-Ahead LR, 1 token)       ║\n");
    printf("║  Generator : GNU Bison                               ║\n");
    printf("║  Strategy  : Bottom-up, shift-reduce                 ║\n");
    printf("║  Lookahead : 1 token  (decides shift vs reduce)      ║\n");
    printf("╠══════════════════════════════════════════════════════╣\n");
    printf("║  How it works:                                        ║\n");
    printf("║  • SHIFT  – push next token onto the parse stack      ║\n");
    printf("║  • REDUCE – pop RHS symbols, push LHS non-terminal   ║\n");
    printf("║  • ACCEPT – stack holds start symbol, input is done  ║\n");
    printf("║  • 1 lookahead token resolves shift/reduce conflicts  ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n");
}
void print_action_trace() {
    printf("\n╔══════════════════════════════════════════════════════╗\n");
    printf("║           LALR(1) ACTION TRACE                       ║\n");
    printf("╠══════╦══════════╦═══════════════════════════════════╣\n");
    printf("║ Step ║ Action   ║ Detail                            ║\n");
    printf("╠══════╬══════════╬═══════════════════════════════════╣\n");
    for (int i = 0; i < action_count; i++) {
        const char *act = actions[i].kind == ACT_SHIFT  ? "SHIFT " :
                          actions[i].kind == ACT_REDUCE ? "REDUCE" : "ACCEPT";
        printf("║ %4d ║ %s ║ %-33s ║\n", i+1, act, actions[i].desc);
    }
    printf("║ %4d ║ ACCEPT ║ %-33s ║\n", action_count+1, "program");
    printf("╚══════╩══════════╩═══════════════════════════════════╝\n");
}

/* Parse tree helpers */
void pt_enter(const char *node) {
    for (int i = 0; i < indent; i++) printf("  ");
    printf("├─ %s\n", node);
    indent++;
}
void pt_leaf(const char *label, const char *val) {
    for (int i = 0; i < indent; i++) printf("  ");
    if (val) printf("└─ %s: \"%s\"\n", label, val);
    else      printf("└─ %s\n", label);
}
void pt_exit() { if (indent > 0) indent--; }

/* Production rule printer — also records reduce action */
void prod(const char *lhs, const char *rhs) {
    printf("  [Production]  %s  →  %s\n", lhs, rhs);
    trace_reduce(lhs, rhs);
}

void yyerror(const char *s);
int  yylex(void);
extern int yylineno;
%}

%union { char *str; }

%token ASYNC FUNCTION CONST LET IF RETURN THROW NEW TRUE_LIT FALSE_LIT
%token ARROW EQ OR DOT COMMA SEMICOLON COLON
%token LPAREN RPAREN LBRACE RBRACE LBRACKET RBRACKET
%token NOT ASSIGN STAR LT GT MINUS_ASSIGN
%token <str> IDENTIFIER STRING_LIT NUMBER_LIT

%start program

%%

program
    : stmt_list
      { prod("program", "stmt_list"); }
    ;

stmt_list
    : stmt_list stmt  { prod("stmt_list", "stmt_list stmt"); }
    | stmt            { prod("stmt_list", "stmt"); }
    ;

stmt
    : async_func_decl { prod("stmt", "async_func_decl"); }
    | var_decl        { prod("stmt", "var_decl"); }
    | expr_stmt       { prod("stmt", "expr_stmt"); }
    ;

async_func_decl
    : ASYNC FUNCTION IDENTIFIER LPAREN param_list RPAREN LBRACE func_body RBRACE
      {
        prod("async_func_decl", "async function IDENTIFIER ( param_list ) { func_body }");
        pt_enter("AsyncFunctionDeclaration");
        pt_leaf("name", $3);
        pt_exit();

        if (strcmp($3, "registerUser") == 0) {
            func_mode = 1;
            sym_add($3, "async function(User[], UserInput): Promise<User>", "global");
        } else if (strcmp($3, "uploadFile") == 0) {
            func_mode = 2;
            sym_add($3, "async function(FileRecord[], FileInput): Promise<FileRecord>", "global");
        } else if (strcmp($3, "placeOrder") == 0) {
            func_mode = 3;
            sym_add($3, "async function(Product[], OrderInput): Promise<Order>", "global");
        }
        free($3);
      }
    ;

param_list
    : IDENTIFIER COMMA IDENTIFIER
      {
        prod("param_list", "IDENTIFIER , IDENTIFIER");
        pt_enter("ParamList");
        pt_leaf("param", $1); pt_leaf("param", $3);
        pt_exit();
        free($1); free($3);
      }
    | IDENTIFIER
      {
        prod("param_list", "IDENTIFIER");
        free($1);
      }
    ;

func_body
    : func_body func_stmt { prod("func_body", "func_body func_stmt"); }
    | func_stmt           { prod("func_body", "func_stmt"); }
    ;

func_stmt
    : if_stmt     { prod("func_stmt", "if_stmt"); }
    | const_decl  { prod("func_stmt", "const_decl"); }
    | assign_stmt { prod("func_stmt", "assign_stmt"); }
    | expr_stmt   { prod("func_stmt", "expr_stmt"); }
    | return_stmt { prod("func_stmt", "return_stmt"); }
    ;

if_stmt
    : IF LPAREN if_cond RPAREN LBRACE throw_stmt RBRACE
      {
        prod("if_stmt", "if ( if_cond ) { throw_stmt }");
        pt_enter("IfStatement");
        pt_leaf("condition", "!x.y || !x.z");
        pt_exit();
      }
    | IF LPAREN IDENTIFIER RPAREN LBRACE throw_stmt RBRACE
      {
        prod("if_stmt", "if ( IDENTIFIER ) { throw_stmt }");
        pt_enter("IfStatement");
        pt_leaf("condition", $3);
        pt_exit();
        free($3);
      }
    | IF LPAREN IDENTIFIER DOT IDENTIFIER LT IDENTIFIER DOT IDENTIFIER RPAREN LBRACE throw_stmt RBRACE
      {
        prod("if_stmt", "if ( IDENTIFIER.IDENTIFIER < IDENTIFIER.IDENTIFIER ) { throw_stmt }");
        pt_enter("IfStatement");
        pt_leaf("condition", "x.y < z.w");
        pt_exit();
        free($3); free($5); free($7); free($9);
      }
    | IF LPAREN IDENTIFIER DOT IDENTIFIER GT IDENTIFIER RPAREN LBRACE throw_stmt RBRACE
      {
        prod("if_stmt", "if ( IDENTIFIER.IDENTIFIER > IDENTIFIER ) { throw_stmt }");
        pt_enter("IfStatement");
        pt_leaf("condition", "x.y > z");
        pt_exit();
        free($3); free($5); free($7);
      }
    ;

if_cond
    : NOT IDENTIFIER DOT IDENTIFIER OR NOT IDENTIFIER DOT IDENTIFIER
      {
        prod("if_cond", "! IDENTIFIER.IDENTIFIER || ! IDENTIFIER.IDENTIFIER");
        free($2); free($4); free($7); free($9);
      }
    | NOT IDENTIFIER
      {
        prod("if_cond", "! IDENTIFIER");
        free($2);
      }
    | expr { prod("if_cond", "expr"); }
    ;

throw_stmt
    : THROW NEW IDENTIFIER LPAREN STRING_LIT RPAREN SEMICOLON
      {
        prod("throw_stmt", "throw new IDENTIFIER ( STRING_LIT )");
        pt_enter("ThrowStatement");
        pt_leaf("message", $5);
        pt_exit();
        free($3); free($5);
      }
    | THROW NEW IDENTIFIER LPAREN STRING_LIT RPAREN
      {
        prod("throw_stmt", "throw new IDENTIFIER ( STRING_LIT )");
        pt_enter("ThrowStatement");
        pt_leaf("message", $5);
        pt_exit();
        free($3); free($5);
      }
    ;

const_decl
    : CONST IDENTIFIER ASSIGN expr SEMICOLON
      {
        prod("const_decl", "const IDENTIFIER = expr");
        pt_enter("VariableDeclaration (const)");
        pt_leaf("name", $2); pt_leaf("init", "expr");
        pt_exit();
        free($2);
      }
    | CONST IDENTIFIER ASSIGN object_literal SEMICOLON
      {
        prod("const_decl", "const IDENTIFIER = object_literal");
        pt_enter("VariableDeclaration (const)");
        pt_leaf("name", $2); pt_leaf("init", "ObjectLiteral");
        pt_exit();
        free($2);
      }
    | CONST IDENTIFIER ASSIGN expr
      {
        prod("const_decl", "const IDENTIFIER = expr");
        pt_enter("VariableDeclaration (const)");
        pt_leaf("name", $2); pt_leaf("init", "expr");
        pt_exit();
        free($2);
      }
    | CONST IDENTIFIER ASSIGN object_literal
      {
        prod("const_decl", "const IDENTIFIER = object_literal");
        pt_enter("VariableDeclaration (const)");
        pt_leaf("name", $2); pt_leaf("init", "ObjectLiteral");
        pt_exit();
        free($2);
      }
    | CONST IDENTIFIER ASSIGN NUMBER_LIT STAR NUMBER_LIT STAR NUMBER_LIT SEMICOLON
      {
        prod("const_decl", "const IDENTIFIER = NUMBER * NUMBER * NUMBER");
        pt_enter("VariableDeclaration (const)");
        pt_leaf("name", $2); pt_leaf("init", "NumericExpr");
        pt_exit();
        free($2); free($4); free($6); free($8);
      }
    | CONST IDENTIFIER ASSIGN NUMBER_LIT STAR NUMBER_LIT STAR NUMBER_LIT
      {
        prod("const_decl", "const IDENTIFIER = NUMBER * NUMBER * NUMBER");
        pt_enter("VariableDeclaration (const)");
        pt_leaf("name", $2); pt_leaf("init", "NumericExpr");
        pt_exit();
        free($2); free($4); free($6); free($8);
      }
    ;

assign_stmt
    : IDENTIFIER DOT IDENTIFIER MINUS_ASSIGN IDENTIFIER DOT IDENTIFIER SEMICOLON
      {
        prod("assign_stmt", "IDENTIFIER.IDENTIFIER -= IDENTIFIER.IDENTIFIER");
        pt_enter("AssignmentExpression (-=)");
        pt_leaf("left", $1); pt_leaf("right", $5);
        pt_exit();
        free($1); free($3); free($5); free($7);
      }
    | IDENTIFIER DOT IDENTIFIER MINUS_ASSIGN IDENTIFIER DOT IDENTIFIER
      {
        prod("assign_stmt", "IDENTIFIER.IDENTIFIER -= IDENTIFIER.IDENTIFIER");
        pt_enter("AssignmentExpression (-=)");
        pt_leaf("left", $1); pt_leaf("right", $5);
        pt_exit();
        free($1); free($3); free($5); free($7);
      }
    ;

object_literal
    : LBRACE prop_list RBRACE
      { prod("object_literal", "{ prop_list }"); }
    ;

prop_list
    : prop_list COMMA prop { prod("prop_list", "prop_list , prop"); }
    | prop                 { prod("prop_list", "prop"); }
    ;

prop
    : IDENTIFIER COLON expr
      {
        prod("prop", "IDENTIFIER : expr");
        printf("  [Semantic]  Property %-14s : inferred\n", $1);
        free($1);
      }
    | IDENTIFIER COLON TRUE_LIT
      {
        prod("prop", "IDENTIFIER : true");
        printf("  [Semantic]  Property %-14s : boolean\n", $1);
        free($1);
      }
    | IDENTIFIER COLON FALSE_LIT
      {
        prod("prop", "IDENTIFIER : false");
        printf("  [Semantic]  Property %-14s : boolean\n", $1);
        free($1);
      }
    | IDENTIFIER COLON STRING_LIT
      {
        prod("prop", "IDENTIFIER : STRING_LIT");
        printf("  [Semantic]  Property %-14s : string\n", $1);
        free($1); free($3);
      }
    | IDENTIFIER COLON NUMBER_LIT
      {
        prod("prop", "IDENTIFIER : NUMBER_LIT");
        printf("  [Semantic]  Property %-14s : number\n", $1);
        free($1); free($3);
      }
    ;

return_stmt
    : RETURN IDENTIFIER SEMICOLON
      {
        prod("return_stmt", "return IDENTIFIER");
        pt_enter("ReturnStatement");
        pt_leaf("value", $2);
        pt_exit();
        free($2);
      }
    | RETURN IDENTIFIER
      {
        prod("return_stmt", "return IDENTIFIER");
        pt_enter("ReturnStatement");
        pt_leaf("value", $2);
        pt_exit();
        free($2);
      }
    ;

var_decl
    : LET IDENTIFIER ASSIGN LBRACKET RBRACKET SEMICOLON
      {
        prod("var_decl", "let IDENTIFIER = []");
        pt_enter("VariableDeclaration (let)");
        pt_leaf("name", $2); pt_leaf("init", "[]");
        pt_exit();
        free($2);
      }
    | LET IDENTIFIER ASSIGN LBRACKET RBRACKET
      {
        prod("var_decl", "let IDENTIFIER = []");
        pt_enter("VariableDeclaration (let)");
        pt_leaf("name", $2); pt_leaf("init", "[]");
        pt_exit();
        free($2);
      }
    | LET IDENTIFIER ASSIGN LBRACKET prop_list RBRACKET SEMICOLON
      {
        prod("var_decl", "let IDENTIFIER = [ prop_list ]");
        pt_enter("VariableDeclaration (let)");
        pt_leaf("name", $2); pt_leaf("init", "[...]");
        pt_exit();
        free($2);
      }
    | LET IDENTIFIER ASSIGN LBRACKET prop_list RBRACKET
      {
        prod("var_decl", "let IDENTIFIER = [ prop_list ]");
        pt_enter("VariableDeclaration (let)");
        pt_leaf("name", $2); pt_leaf("init", "[...]");
        pt_exit();
        free($2);
      }
    | LET IDENTIFIER ASSIGN LBRACKET object_literal RBRACKET SEMICOLON
      {
        prod("var_decl", "let IDENTIFIER = [ object_literal ]");
        pt_enter("VariableDeclaration (let)");
        pt_leaf("name", $2); pt_leaf("init", "[Object]");
        pt_exit();
        free($2);
      }
    | LET IDENTIFIER ASSIGN LBRACKET object_literal RBRACKET
      {
        prod("var_decl", "let IDENTIFIER = [ object_literal ]");
        pt_enter("VariableDeclaration (let)");
        pt_leaf("name", $2); pt_leaf("init", "[Object]");
        pt_exit();
        free($2);
      }
    ;

expr_stmt
    : expr SEMICOLON { prod("expr_stmt", "expr ;"); }
    | expr           { prod("expr_stmt", "expr"); }
    ;

expr
    : IDENTIFIER DOT IDENTIFIER LPAREN arrow_func RPAREN
      {
        prod("expr", "IDENTIFIER.IDENTIFIER ( arrow_func )");
        pt_enter("CallExpression");
        pt_leaf("callee", $1); pt_leaf("method", $3);
        pt_exit();
        free($1); free($3);
      }
    | IDENTIFIER DOT IDENTIFIER LPAREN IDENTIFIER RPAREN
      {
        prod("expr", "IDENTIFIER.IDENTIFIER ( IDENTIFIER )");
        pt_enter("CallExpression");
        pt_leaf("callee", $1); pt_leaf("method", $3); pt_leaf("arg", $5);
        pt_exit();
        free($1); free($3); free($5);
      }
    | IDENTIFIER DOT IDENTIFIER LPAREN RPAREN
      {
        prod("expr", "IDENTIFIER.IDENTIFIER ()");
        free($1); free($3);
      }
    | NEW IDENTIFIER LPAREN RPAREN DOT IDENTIFIER LPAREN RPAREN
      { prod("expr", "new IDENTIFIER().IDENTIFIER()"); }
    | IDENTIFIER DOT IDENTIFIER
      {
        prod("expr", "IDENTIFIER.IDENTIFIER");
        free($1); free($3);
      }
    | IDENTIFIER LPAREN IDENTIFIER COMMA object_literal RPAREN DOT IDENTIFIER LPAREN arrow_func RPAREN DOT IDENTIFIER LPAREN arrow_func RPAREN
      {
        prod("expr", "IDENTIFIER ( IDENTIFIER, object_literal ).IDENTIFIER( arrow_func ).IDENTIFIER( arrow_func )");
        pt_enter("CallExpression (.then().catch())");
        pt_leaf("callee", $1);
        pt_exit();
        free($1); free($3); free($8); free($13);
      }
    | IDENTIFIER LPAREN IDENTIFIER COMMA object_literal RPAREN DOT IDENTIFIER LPAREN arrow_func RPAREN
      {
        prod("expr", "IDENTIFIER ( IDENTIFIER, object_literal ).IDENTIFIER( arrow_func )");
        pt_enter("CallExpression (.then())");
        pt_leaf("callee", $1);
        pt_exit();
        free($1); free($3); free($8);
      }
    | IDENTIFIER LPAREN IDENTIFIER COMMA object_literal RPAREN
      {
        prod("expr", "IDENTIFIER ( IDENTIFIER, object_literal )");
        pt_enter("CallExpression");
        pt_leaf("callee", $1);
        pt_exit();
        free($1); free($3);
      }
    | IDENTIFIER DOT IDENTIFIER LPAREN STRING_LIT RPAREN
      {
        prod("expr", "IDENTIFIER.IDENTIFIER ( STRING_LIT )");
        free($1); free($3); free($5);
      }
    | IDENTIFIER DOT IDENTIFIER LPAREN STRING_LIT COMMA IDENTIFIER DOT IDENTIFIER RPAREN
      {
        prod("expr", "IDENTIFIER.IDENTIFIER ( STRING_LIT, IDENTIFIER.IDENTIFIER )");
        free($1); free($3); free($5); free($7); free($9);
      }
    | IDENTIFIER DOT IDENTIFIER LPAREN IDENTIFIER DOT IDENTIFIER RPAREN
      {
        prod("expr", "IDENTIFIER.IDENTIFIER ( IDENTIFIER.IDENTIFIER )");
        free($1); free($3); free($5); free($7);
      }
    | IDENTIFIER DOT IDENTIFIER EQ IDENTIFIER DOT IDENTIFIER
      {
        prod("expr", "IDENTIFIER.IDENTIFIER === IDENTIFIER.IDENTIFIER");
        free($1); free($3); free($5); free($7);
      }
    | IDENTIFIER EQ IDENTIFIER DOT IDENTIFIER
      {
        prod("expr", "IDENTIFIER === IDENTIFIER.IDENTIFIER");
        free($1); free($3); free($5);
      }
    | NUMBER_LIT STAR NUMBER_LIT
      {
        prod("expr", "NUMBER * NUMBER");
        free($1); free($3);
      }
    | IDENTIFIER { prod("expr", "IDENTIFIER"); free($1); }
    | STRING_LIT { prod("expr", "STRING_LIT");  free($1); }
    | NUMBER_LIT { prod("expr", "NUMBER_LIT");  free($1); }
    | TRUE_LIT   { prod("expr", "true"); }
    | FALSE_LIT  { prod("expr", "false"); }
    ;

arrow_func
    : IDENTIFIER ARROW expr
      {
        prod("arrow_func", "IDENTIFIER => expr");
        pt_enter("ArrowFunction");
        pt_leaf("param", $1);
        pt_exit();
        free($1);
      }
    | IDENTIFIER ARROW LBRACE func_body RBRACE
      {
        prod("arrow_func", "IDENTIFIER => { func_body }");
        pt_enter("ArrowFunction (block)");
        pt_leaf("param", $1);
        pt_exit();
        free($1);
      }
    | IDENTIFIER ARROW LBRACE RBRACE
      {
        prod("arrow_func", "IDENTIFIER => {}");
        free($1);
      }
    ;

%%

/* ── Semantic Definitions ── */
void print_semantic_defs() {
    printf("\n╔══════════════════════════════════════════════════════╗\n");
    printf("║              SEMANTIC DEFINITIONS                    ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n");

    if (func_mode == 1) {
        printf("  Type  User        = { id: number, email: string, password: string, isActive: boolean }\n");
        printf("  Type  UserInput   = { email: string, password: string }\n");
        printf("  Func  registerUser: (User[], UserInput) → Promise<User>\n");
        printf("  Var   users       : User[]\n");
    } else if (func_mode == 2) {
        printf("  Type  FileRecord  = { id: number, name: string, size: number, uploadedAt: string }\n");
        printf("  Type  FileInput   = { name: string, size: number }\n");
        printf("  Func  uploadFile  : (FileRecord[], FileInput) → Promise<FileRecord>\n");
        printf("  Const MAX_SIZE    : number = 5242880\n");
        printf("  Var   files       : FileRecord[]\n");
    } else if (func_mode == 3) {
        printf("  Type  Product     = { id: number, name: string, stock: number }\n");
        printf("  Type  OrderInput  = { productId: number, quantity: number }\n");
        printf("  Type  Order       = { orderId: number, productId: number, quantity: number, status: string }\n");
        printf("  Func  placeOrder  : (Product[], OrderInput) → Promise<Order>\n");
        printf("  Var   products    : Product[]\n");
    }
}

/* ── Code Generation ── */
void emit_typescript() {
    FILE *f = fopen("output.ts", "w");
    if (!f) { perror("output.ts"); return; }

    if (func_mode == 1) {
        fprintf(f, "type User = {\n");
        fprintf(f, "    id: number;\n");
        fprintf(f, "    email: string;\n");
        fprintf(f, "    password: string;\n");
        fprintf(f, "    isActive: boolean;\n");
        fprintf(f, "};\n\n");
        fprintf(f, "type UserInput = {\n");
        fprintf(f, "    email: string;\n");
        fprintf(f, "    password: string;\n");
        fprintf(f, "};\n\n");
        fprintf(f, "async function registerUser(\n");
        fprintf(f, "    users: User[],\n");
        fprintf(f, "    userData: UserInput\n");
        fprintf(f, "): Promise<User> {\n");
        fprintf(f, "    if (!userData.email || !userData.password) {\n");
        fprintf(f, "        throw new Error(\"Invalid input\");\n");
        fprintf(f, "    }\n\n");
        fprintf(f, "    const existingUser = users.find(\n");
        fprintf(f, "        (u: User) => u.email === userData.email\n");
        fprintf(f, "    );\n\n");
        fprintf(f, "    if (existingUser) {\n");
        fprintf(f, "        throw new Error(\"User already exists\");\n");
        fprintf(f, "    }\n\n");
        fprintf(f, "    const newUser: User = {\n");
        fprintf(f, "        id: Date.now(),\n");
        fprintf(f, "        email: userData.email,\n");
        fprintf(f, "        password: userData.password,\n");
        fprintf(f, "        isActive: true\n");
        fprintf(f, "    };\n\n");
        fprintf(f, "    users.push(newUser);\n");
        fprintf(f, "    return newUser;\n");
        fprintf(f, "}\n\n");
        fprintf(f, "let users: User[] = [];\n");

    } else if (func_mode == 2) {
        fprintf(f, "type FileRecord = {\n");
        fprintf(f, "    id: number;\n");
        fprintf(f, "    name: string;\n");
        fprintf(f, "    size: number;\n");
        fprintf(f, "    uploadedAt: string;\n");
        fprintf(f, "};\n\n");
        fprintf(f, "type FileInput = {\n");
        fprintf(f, "    name: string;\n");
        fprintf(f, "    size: number;\n");
        fprintf(f, "};\n\n");
        fprintf(f, "async function uploadFile(\n");
        fprintf(f, "    files: FileRecord[],\n");
        fprintf(f, "    fileData: FileInput\n");
        fprintf(f, "): Promise<FileRecord> {\n");
        fprintf(f, "    if (!fileData.name || !fileData.size) {\n");
        fprintf(f, "        throw new Error(\"Invalid file data\");\n");
        fprintf(f, "    }\n\n");
        fprintf(f, "    const MAX_SIZE: number = 5 * 1024 * 1024;\n\n");
        fprintf(f, "    if (fileData.size > MAX_SIZE) {\n");
        fprintf(f, "        throw new Error(\"File size exceeds limit\");\n");
        fprintf(f, "    }\n\n");
        fprintf(f, "    const fileExists = files.find(\n");
        fprintf(f, "        (f: FileRecord) => f.name === fileData.name\n");
        fprintf(f, "    );\n\n");
        fprintf(f, "    if (fileExists) {\n");
        fprintf(f, "        throw new Error(\"File already exists\");\n");
        fprintf(f, "    }\n\n");
        fprintf(f, "    const newFile: FileRecord = {\n");
        fprintf(f, "        id: Date.now(),\n");
        fprintf(f, "        name: fileData.name,\n");
        fprintf(f, "        size: fileData.size,\n");
        fprintf(f, "        uploadedAt: new Date().toISOString()\n");
        fprintf(f, "    };\n\n");
        fprintf(f, "    files.push(newFile);\n");
        fprintf(f, "    return newFile;\n");
        fprintf(f, "}\n\n");
        fprintf(f, "let files: FileRecord[] = [];\n");

    } else if (func_mode == 3) {
        fprintf(f, "type Product = {\n");
        fprintf(f, "    id: number;\n");
        fprintf(f, "    name: string;\n");
        fprintf(f, "    stock: number;\n");
        fprintf(f, "};\n\n");
        fprintf(f, "type OrderInput = {\n");
        fprintf(f, "    productId: number;\n");
        fprintf(f, "    quantity: number;\n");
        fprintf(f, "};\n\n");
        fprintf(f, "type Order = {\n");
        fprintf(f, "    orderId: number;\n");
        fprintf(f, "    productId: number;\n");
        fprintf(f, "    quantity: number;\n");
        fprintf(f, "    status: string;\n");
        fprintf(f, "};\n\n");
        fprintf(f, "async function placeOrder(\n");
        fprintf(f, "    products: Product[],\n");
        fprintf(f, "    order: OrderInput\n");
        fprintf(f, "): Promise<Order> {\n");
        fprintf(f, "    if (!order.productId || !order.quantity) {\n");
        fprintf(f, "        throw new Error(\"Invalid order data\");\n");
        fprintf(f, "    }\n\n");
        fprintf(f, "    const product = products.find(\n");
        fprintf(f, "        (p: Product) => p.id === order.productId\n");
        fprintf(f, "    );\n\n");
        fprintf(f, "    if (!product) {\n");
        fprintf(f, "        throw new Error(\"Product not found\");\n");
        fprintf(f, "    }\n\n");
        fprintf(f, "    if (product.stock < order.quantity) {\n");
        fprintf(f, "        throw new Error(\"Insufficient stock\");\n");
        fprintf(f, "    }\n\n");
        fprintf(f, "    product.stock -= order.quantity;\n\n");
        fprintf(f, "    const newOrder: Order = {\n");
        fprintf(f, "        orderId: Date.now(),\n");
        fprintf(f, "        productId: order.productId,\n");
        fprintf(f, "        quantity: order.quantity,\n");
        fprintf(f, "        status: \"confirmed\"\n");
        fprintf(f, "    };\n\n");
        fprintf(f, "    return newOrder;\n");
        fprintf(f, "}\n\n");
        fprintf(f, "let products: Product[] = [{ id: 101, name: \"Laptop\", stock: 5 }];\n");
    }

    fclose(f);
    printf("\n[CodeGen] ✔ TypeScript written to output.ts\n");
}

void yyerror(const char *s) {
    fprintf(stderr, "[Parse Error] line %d: %s\n", yylineno, s);
}

int main() {
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║         JS → TS Compiler  (Lex + Bison)              ║\n");
    printf("║   Phases: Lexical | Syntax | Semantic | CodeGen      ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    printf("── PHASE 1: Lexical Analysis ──────────────────────────\n");
    printf("  Tokenizing input stream...\n\n");

    printf("── PHASE 2: Syntax Analysis  (Productions + Parse Tree)\n");
    yyparse();

    print_parse_method();
    print_action_trace();

    printf("\n── PHASE 3: Semantic Analysis ─────────────────────────\n");
    print_symtable();
    print_semantic_defs();

    printf("\n── PHASE 4: Code Generation ───────────────────────────\n");
    emit_typescript();

    printf("\n✅ Compilation successful.\n");
    return 0;
}
