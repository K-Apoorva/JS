# Phase 4 — Code Generation

## Role

Code generation is the final phase. It walks the AST built by the parser, reads the type information collected by semantic analysis, and emits valid TypeScript into `output.ts`.

This is **not** template-based. Every statement, expression, and type annotation is derived from the actual AST nodes and the symbol table — the compiler generates what it parsed, not a pre-written string.

---

## Entry Point

```c
emit_program(root, out_file);
```

This walks the top-level `N_PROGRAM` node's statement list and dispatches each node to the appropriate emitter.

---

## Step 1 — Emit Type Aliases

```c
emit_type_aliases(f);
```

Walks the type registry built during semantic analysis. For each `TypeDef`:

```c
fprintf(f, "type %s = {\n", td->name);
for each prop:
    fprintf(f, "    %s: %s;\n", prop.key, prop.type);
fprintf(f, "};\n\n");
```

**Example — input.js:**
```typescript
type User = {
    id: number;
    email: string;
    password: string;
    isActive: boolean;
};
```

The property types come directly from `prop_val_type()` inference during semantic analysis — not hardcoded.

---

## Step 2 — Emit Function Declaration

```c
emit_func(node, f);
```

Reads param types from the symbol table via `var_type_in(param_name, func_name)` and the return type from `infer_return_type(func_node)`.

```c
if (n->ival) fprintf(f, "async ");
fprintf(f, "function %s(\n", n->sval);
for each param:
    fprintf(f, "    %s: %s\n", param_name, ts_param_type(param_name, func_name));
fprintf(f, "): %s {\n", return_type);
emit_block_stmts(body, f, 1);
fprintf(f, "}\n");
```

**Example — input.js:**
```typescript
async function registerUser(
    users: User[],
    userData: UserInput
): Promise<User> {
    ...
}
```

---

## Step 3 — Emit Statements

`emit_stmt(node, f, indent)` dispatches on node kind:

### `N_VAR_DECL`

```c
// determines annotation from node type
const x: User = { ... }    // object literal → type name from derive_type_name
const MAX_SIZE: number = …  // number literal or binary → ": number"
let x: unknown[]            // array → resolved from symbol table
```

### `N_IF`

```c
fprintf(f, "if (");
emit_expr(condition, f);
fprintf(f, ") {\n");
emit_block_stmts(then_branch, f, ind+1);
fprintf(f, "}");
// optional else branch
```

### `N_THROW`

```c
fprintf(f, "throw ");
emit_expr(expr, f);
fprintf(f, ";\n");
```

### `N_RETURN`

```c
fprintf(f, "return ");
emit_expr(expr, f);
fprintf(f, ";\n");
```

### `N_EXPR_STMT`

```c
emit_expr(expr, f);
fprintf(f, ";\n");
```

---

## Step 4 — Emit Expressions

`emit_expr(node, f)` recursively emits any expression node:

| Node kind | Emitted as |
|---|---|
| `N_IDENT` | `name` |
| `N_STR` | `"string"` |
| `N_NUM` | `123` |
| `N_BOOL` | `true` / `false` |
| `N_NULL` | `null` |
| `N_UNARY` | `!expr` |
| `N_BINARY` | `left OP right` |
| `N_ASSIGN` | `left OP= right` |
| `N_MEMBER` | `obj.prop` |
| `N_NEW` | `new Ctor(args)` |
| `N_CALL` | `callee(args)` — with special handling for `.find()` |
| `N_ARROW` | `param => body` |
| `N_OBJECT` | `{ key: value, ... }` |
| `N_ARRAY` | `[elem, ...]` |

### Special case: `.find()` / `.filter()` / `.map()` calls

When the callee is a method call on an array and the method is `find`, `filter`, or `map`, the arrow function argument is annotated with the element type:

```c
// detect array element type from symbol table
const char *at = ts_type_of(obj->sval);  // "User[]"
strip "[]" → elem_type = "User"
// emit arrow param with type annotation
fprintf(f, "(%s: %s) => ", arrow_param, elem_type);
emit_expr(arrow_body, f);
```

**Result:**
```typescript
users.find((u: User) => u.email === userData.email)
```

---

## Step 5 — Emit Top-Level `let` Declarations

```c
emit_top_let(node, f);
```

Finds the array type from the symbol table (first param typed as `X[]` in any function scope):

```typescript
let users: User[] = [];
let files: FileRecord[] = [];
let products: Product[] = [{ id: 101, name: "Laptop", stock: 5 }];
```

---

## Full Transformation Example

**Input (input.js):**
```javascript
async function registerUser(users, userData) {
    if (!userData.email || !userData.password) {
        throw new Error("Invalid input");
    }
    const existingUser = users.find(u => u.email === userData.email);
    if (existingUser) {
        throw new Error("User already exists");
    }
    const newUser = {
        id: Date.now(),
        email: userData.email,
        password: userData.password,
        isActive: true
    };
    users.push(newUser);
    return newUser;
}
let users = [];
```

**Output (output1.ts):**
```typescript
type User = {
    id: number;
    email: string;
    password: string;
    isActive: boolean;
};

async function registerUser(
    users: User[],
    userData: UserInput
): Promise<User> {
    if (!userData.email || !userData.password) {
        throw new Error("Invalid input");
    }
    const existingUser = users.find((u: User) => u.email === userData.email);
    if (existingUser) {
        throw new Error("User already exists");
    }
    const newUser: User = {
        id: Date.now(),
        email: userData.email,
        password: userData.password,
        isActive: true
    };
    users.push(newUser);
    return newUser;
}

let users: User[] = [];
```

Every type annotation — `User`, `User[]`, `UserInput`, `Promise<User>`, `(u: User)`, `: User` on `newUser` — was derived from the AST and symbol table, not written in advance.

---

## Parse Trace Output

After code generation, if a trace filename was passed as an argument, `write_trace()` is called:

```c
write_trace(trace_file, input_label);
```

This walks the same AST a second time and writes a human-readable tree to the trace file, followed by the symbol table and a phase summary. Each input gets its own trace file: `trace_input1.txt`, `trace_input2.txt`, `trace_input3.txt`.

---

## File I/O

```c
FILE *out = fopen("output.ts", "w");
emit_program(root, out);
fclose(out);
```

The output file is only created if parsing succeeded with zero errors. On any syntax error, the compiler aborts before reaching this phase.
