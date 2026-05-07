# Phase 3 — Semantic Analysis

## Role

Semantic analysis runs after the AST is built. It walks the tree and answers the question: **what does this code mean?** Specifically:

- What type does each variable have?
- What type does each function return?
- What are the shapes of the objects being constructed?
- Are there any type-level problems?

The results are stored in a **symbol table** and a **type registry**, which the code generation phase reads to emit typed TypeScript.

---

## SDD and SDT

**SDD (Syntax Directed Definition)** — specifies what attributes are computed for each grammar rule.  
**SDT (Syntax Directed Translation)** — specifies what action is executed when a rule is reduced.

---

## 1. Program

### Production
```
program → stmt_list
```

### SDD
```
program.code = stmt_list.code
```

### SDT
```c
infer_program(root);   // walk all top-level statements
```

The semantic pass is a single post-parse walk of the AST. It processes each top-level statement in order.

---

## 2. Async Function Declaration

### Production
```
func_decl → ASYNC FUNCTION IDENTIFIER ( param_list ) block
```

### SDD
```
IDENTIFIER.lexval  = function name
func.returnType    = Promise<T>  where T = derive_type_name(last const obj in body)
func.paramTypes    = inferred from usage inside body
```

### SDT
```c
infer_param_types(func_node);
var_set(func->sval, "async function(...): Promise<T>", "global");
```

**How param types are inferred:**

The body is scanned for usage patterns:

| Pattern seen | Inference |
|---|---|
| `param.find(...)` or `param.push(...)` | param is an array |
| `param.filter(...)` or `param.map(...)` | param is an array |
| No array usage | param is an input object |

Array element type is derived from the param name by stripping the plural `s`:
- `users` → `User[]`
- `files` → `FileRecord[]` (builtin collision → append `Record`)
- `products` → `Product[]`

Input object type is derived from the param name:
- `userData` → strip `Data` → `User` → append `Input` → `UserInput`
- `fileData` → `FileInput`
- `order` → `OrderInput`

**How return type is inferred:**

The body is scanned for `const newX = { ... }`. The variable name is used to derive the type:
- `newUser` → strip `new` → `User` → return `Promise<User>`
- `newFile` → `File` → builtin collision → `FileRecord` → `Promise<FileRecord>`
- `newOrder` → `Order` → `Promise<Order>`

---

## 3. Parameter List

### Production
```
param_list → IDENTIFIER , IDENTIFIER
           | IDENTIFIER
```

### SDD
```
param.lexval = identifier name
param.scope  = enclosing function name
```

### SDT
```c
// params are stored as N_IDENT nodes in func->left->list
// typed during infer_param_types()
var_set(pname, inferred_type, func->sval);
```

---

## 4. Variable Declaration (const)

### Production
```
var_decl → CONST IDENTIFIER = object_literal
```

### SDD
```
IDENTIFIER.type  = derive_type_name(IDENTIFIER.lexval)
object.typeName  = IDENTIFIER.type
```

### SDT
```c
char *tn = derive_type_name(s->sval);   // "newUser" → "User"
register_object_type(tn, s->left);      // build TypeDef from object literal
var_set(s->sval, tn, func->sval);       // add to symbol table
```

`register_object_type` walks the object literal's property list and infers each property's type:

| Property value | Inferred type |
|---|---|
| `true` / `false` | `boolean` |
| `"string"` | `string` |
| `123` | `number` |
| `Date.now()` | `number` |
| `new Date().toISOString()` | `string` |
| `x * y` (binary) | `number` |
| `param.field` (member access) | `string` (field name heuristic) |

---

## 5. Variable Declaration (let array)

### Production
```
var_decl → LET IDENTIFIER = [ ]
         | LET IDENTIFIER = [ elem_list ]
```

### SDD
```
IDENTIFIER.type = "__let_array__"   (resolved at emit time)
```

### SDT
```c
var_set(s->sval, "__let_array__", "global");
// if array contains object literal elements, register their type
register_object_type(elem_type_name, first_element_node);
```

---

## 6. If Statement

### Production
```
if_stmt → IF ( expr ) block
        | IF ( expr ) block ELSE block
```

### SDD
```
expr.type = boolean   (enforced by JS semantics — any value is truthy/falsy)
```

### SDT
No explicit type check is emitted — the condition is reproduced as-is in the output. The semantic value is that the condition structure is preserved in the AST for accurate code generation.

---

## 7. Throw Statement

### Production
```
throw_stmt → THROW expr
```

### SDD
```
expr.type = Error (or any throwable)
```

### SDT
The throw node is stored in the AST and emitted verbatim. No type constraint is enforced at this phase.

---

## 8. Return Statement

### Production
```
return_stmt → RETURN expr
```

### SDD
```
return.type = expr.type
```

### SDT
The return value's variable name is used to confirm the return type matches the inferred `Promise<T>`. If the returned identifier matches the `newX` variable, the return type is confirmed.

---

## 9. Object Literal

### Production
```
object_literal → { prop_list }
prop           → IDENTIFIER : expr
```

### SDD
```
prop.key   = IDENTIFIER.lexval
prop.type  = infer from expr node kind
object.shape = { prop.key: prop.type, ... }
```

### SDT
```c
// called during infer_param_types when const newX = { ... } is found
static void register_object_type(const char *tname, Node *obj) {
    TypeDef *td = type_new(tname);
    for each prop in obj->list:
        type_add_prop(td, prop->sval, prop_val_type(prop->left));
}
```

The `TypeDef` struct stores the type name and all its properties with their inferred types. This is later used by code generation to emit `type X = { ... }`.

---

## 10. Arrow Function

### Production
```
arrow_func → IDENTIFIER => assign_expr
           | IDENTIFIER => block
```

### SDD
```
param.lexval   = IDENTIFIER
body.type      = assign_expr.type
```

### SDT
When the arrow function appears as an argument to `.find()`, `.filter()`, or `.map()`, the code generator annotates the parameter with the array's element type:
```typescript
(u: User) => u.email === userData.email
```

---

## 11. Binary Expression

### Production
```
eq_expr  → eq_expr === rel_expr
rel_expr → rel_expr < add_expr
add_expr → add_expr + mul_expr
mul_expr → mul_expr * unary_expr
```

### SDD
```
===, !==, <, >, <=, >=, ||, &&  →  expr.type = boolean
+, -, *, /, %                   →  expr.type = number
```

### SDT
Used by `prop_val_type()` — if a property value is a binary expression, its type is inferred as `number`.

---

## 12. Identifier Expression

### Production
```
primary_expr → IDENTIFIER
```

### SDD
```
expr.type = var_type(IDENTIFIER.lexval)
```

### SDT
```c
const char *var_type(const char *name) {
    // linear scan of symbol table, returns type string
}
```

---

## Symbol Table

The symbol table is a flat array of `VarEntry` structs:

```c
typedef struct {
    char name[64];
    char type[64];
    char scope[32];
} VarEntry;
```

`var_set(name, type, scope)` inserts or updates — matching on both name **and** scope so a param named `users` in function scope doesn't overwrite a global `let users`.

`var_type(name)` returns the first match by name (used during expression type lookup).

`var_type_in(name, scope)` returns a scope-specific match (used during param type lookup in codegen).

---

## Type Registry

The type registry is a flat array of `TypeDef` structs:

```c
typedef struct {
    char name[64];
    Prop props[MAX_PROPS];
    int  nprops;
} TypeDef;
```

Each `TypeDef` represents one TypeScript type alias. Properties are added by `type_add_prop(td, key, type)`. The registry is walked by code generation to emit all `type X = { ... }` declarations.

---

## Semantic Error Checks

After inference, the following are checked:

| Check | Error message |
|---|---|
| Function has no parameters | `function 'X': no parameters — cannot infer types` |
| No `const new* = {...}` in body | `function 'X': return type unknown` |
| Property type could not be inferred | `type 'X': property 'Y' type could not be inferred` |

Errors are collected in the same error array as syntax errors and printed in the final error table.

---

## Symbol Table Output (example — input.js)

```
╔════════════════╦══════════════════════════════╦══════════════╗
║ Identifier     ║ Type                         ║ Scope        ║
╠════════════════╬══════════════════════════════╬══════════════╣
║ newUser        ║ User                         ║ registerUser ║
║ users          ║ User[]                       ║ registerUser ║
║ userData       ║ UserInput                    ║ registerUser ║
║ registerUser   ║ async function(...): Promise<User> ║ global  ║
╚════════════════╩══════════════════════════════╩══════════════╝
```
