# Phase 2 — Syntax Analysis

## Role

The syntax analysis phase receives the token stream from the lexer and checks whether the tokens form a structurally valid JavaScript program according to the grammar defined in `parser.y`. It builds an **Abstract Syntax Tree (AST)** in memory that all later phases operate on.

Implemented using **GNU Bison**, which generates an **LALR(1)** bottom-up parser.

---

## Parsing Method: LALR(1)

| Term | Meaning |
|---|---|
| L | Reads input left to right |
| R | Builds rightmost derivation in reverse (bottom-up) |
| 1 | Uses 1 lookahead token to decide shift vs reduce |

The parser maintains a **stack**. It either:
- **SHIFTs** — pushes the next token onto the stack
- **REDUCEs** — pops a sequence matching a grammar rule's RHS and pushes the LHS non-terminal
- **ACCEPTs** — the stack holds the start symbol and input is exhausted

The 1 lookahead token resolves ambiguity: when the parser could either shift or reduce, it peeks at the next token to decide.

---

## Grammar Rules

The grammar is general — it describes JS constructs structurally, not specific token sequences.

### Top Level

```
program     → stmt_list
stmt_list   → stmt_list stmt
            | stmt
stmt        → func_decl
            | var_decl
            | if_stmt
            | throw_stmt
            | return_stmt
            | expr_stmt
            | block
```

### Function Declaration

```
func_decl   → ASYNC FUNCTION IDENTIFIER ( param_list ) block
            | FUNCTION IDENTIFIER ( param_list ) block

param_list  → param_list , IDENTIFIER
            | IDENTIFIER
            | ε
```

### Block and Statements

```
block       → { stmt_list }
            | { }

var_decl    → CONST IDENTIFIER = expr ;
            | CONST IDENTIFIER = expr
            | LET IDENTIFIER = expr ;
            | LET IDENTIFIER = expr
            | LET IDENTIFIER ;

if_stmt     → IF ( expr ) block ELSE block
            | IF ( expr ) block

throw_stmt  → THROW expr ;
            | THROW expr

return_stmt → RETURN expr ;
            | RETURN expr
            | RETURN ;
```

### Expression Hierarchy (precedence low → high)

```
expr          → assign_expr
assign_expr   → postfix_expr ASSIGN_OP assign_expr
              | postfix_expr = assign_expr
              | or_expr
or_expr       → or_expr || and_expr
              | and_expr
and_expr      → and_expr && eq_expr
              | eq_expr
eq_expr       → eq_expr === rel_expr
              | eq_expr !== rel_expr
              | rel_expr
rel_expr      → rel_expr < add_expr
              | rel_expr > add_expr
              | rel_expr <= add_expr
              | rel_expr >= add_expr
              | add_expr
add_expr      → add_expr + mul_expr
              | add_expr - mul_expr
              | mul_expr
mul_expr      → mul_expr * unary_expr
              | mul_expr / unary_expr
              | unary_expr
unary_expr    → ! unary_expr
              | postfix_expr
postfix_expr  → postfix_expr . IDENTIFIER ( arg_list )   ← method call
              | postfix_expr . IDENTIFIER                 ← property access
              | postfix_expr ( arg_list )                 ← function call
              | primary_expr
primary_expr  → IDENTIFIER
              | STRING_LIT | NUMBER_LIT | TRUE_LIT | FALSE_LIT | NULL_LIT
              | NEW IDENTIFIER ( arg_list )
              | object_literal
              | array_literal
              | arrow_func
              | ( expr )
```

### Arrow Functions

```
arrow_func  → IDENTIFIER => assign_expr
            | IDENTIFIER => block
            | ( param_list ) => block
```

### Object and Array Literals

```
object_literal → { prop_list }
               | { }
prop_list      → prop_list , prop
               | prop
prop           → IDENTIFIER : expr
               | STRING_LIT : expr

array_literal  → [ elem_list ]
               | [ ]
elem_list      → elem_list , expr
               | expr
```

---

## AST Node Types

Every grammar rule builds an AST node. The node kinds are:

| Kind | Represents |
|---|---|
| `N_PROGRAM` | root of the tree |
| `N_FUNC` | function declaration (`sval`=name, `ival`=async flag) |
| `N_VAR_DECL` | `const`/`let` declaration (`ival`=1 for const) |
| `N_IF` | if statement (`left`=condition, `right`=then, `extra`=else) |
| `N_THROW` | throw statement |
| `N_RETURN` | return statement |
| `N_EXPR_STMT` | expression used as statement |
| `N_BLOCK` | `{ }` block (`list`=statements) |
| `N_CALL` | function/method call (`left`=callee, `list`=args) |
| `N_MEMBER` | property access (`left`=object, `right`=property) |
| `N_BINARY` | binary expression (`sval`=operator) |
| `N_UNARY` | unary expression (`sval`=operator) |
| `N_ASSIGN` | assignment (`sval`=operator) |
| `N_ARROW` | arrow function (`left`=param, `right`=body) |
| `N_OBJECT` | object literal (`list`=props) |
| `N_PROP` | object property (`sval`=key, `left`=value) |
| `N_ARRAY` | array literal (`list`=elements) |
| `N_NEW` | `new Ctor(args)` |
| `N_IDENT` | identifier |
| `N_STR` | string literal |
| `N_NUM` | number literal |
| `N_BOOL` | boolean literal |
| `N_NULL` | null |

---

## Concrete Parse Example

Input:
```javascript
const existingUser = users.find(u => u.email === userData.email);
```

Parse steps:
```
SHIFT const
SHIFT existingUser (IDENTIFIER)
SHIFT =
  SHIFT users (IDENTIFIER)         → primary_expr → postfix_expr
  SHIFT .
  SHIFT find (IDENTIFIER)          → postfix_expr.find
  SHIFT (
    SHIFT u (IDENTIFIER)           → primary_expr
    SHIFT =>
    SHIFT u.email === userData.email  → binary expr (===)
    REDUCE: arrow_func → IDENTIFIER => assign_expr
  SHIFT )
  REDUCE: postfix_expr → postfix_expr.IDENTIFIER(arg_list)  [users.find(arrow)]
REDUCE: var_decl → CONST IDENTIFIER = expr
```

Resulting AST fragment:
```
[VarDecl] const existingUser
  [Call]
    [Callee]
      [Member] users.find
    [Args]
      [ArrowFunc]
        [Param] u
        [Body]
          [Binary] ===
            [Member] u.email
            [Member] userData.email
```

---

## Error Handling

Bison's `%define parse.error verbose` produces descriptive messages:
```
syntax error, unexpected ASSIGN, expecting IDENTIFIER
```

Recovery rules allow parsing to continue after a bad statement:
```bison
stmt : error SEMICOLON { $$ = NULL; yyerrok; }
     | error RBRACE    { $$ = NULL; yyerrok; }
     | error LBRACE    { $$ = NULL; yyerrok; }
```

`yyerrok` resets error mode so subsequent errors are independently reported.

---

## Conflict Resolution

The grammar has shift/reduce conflicts from two inherent JS ambiguities:

1. **Dangling else** — resolved with `%prec LOWER_THAN_ELSE` on the no-else rule (Bison defaults to shift, which is correct: `else` binds to nearest `if`)
2. **Arrow function body greediness** — `x => expr` where `expr` could consume `||`/`&&` — resolved with `%prec ARROW` (lowest precedence) so the arrow body stops at the right boundary

---

## Generated Files

```bash
bison -d parser.y
```

Produces:
- `parser.tab.c` — the parser implementation
- `parser.tab.h` — token type constants shared with the lexer
