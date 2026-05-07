# JS → TS Compiler — Project Overview

A source-to-source compiler that converts a subset of JavaScript into fully typed TypeScript.
Built with **Flex** (lexer), **Bison** (parser), and **C** (semantic analysis + code generation).

---

## Project Structure

| File | Purpose |
|---|---|
| `lexer.l` | Flex scanner rules — tokenises JS source |
| `parser.y` | Bison grammar + semantic analysis + code generation |
| `ast.h / ast.c` | AST node definitions and constructors |
| `input.js` | Sample input 1 — `registerUser` |
| `input2.js` | Sample input 2 — `uploadFile` |
| `input3.js` | Sample input 3 — `placeOrder` |
| `input_error.js` | Sample input with intentional syntax errors |
| `Makefile` | Build and run automation |

### Auto-generated files

| File | Created by |
|---|---|
| `lex.yy.c` | Flex |
| `parser.tab.c / .h` | Bison |
| `js2ts` | GCC |

### Output files

| File | Contents |
|---|---|
| `output.ts` | Generated TypeScript (current run) |
| `output1/2/3.ts` | Saved outputs for each input |
| `trace_input1/2/3.txt` | Parse trace for each input |

---

## Compiler Pipeline

```
input.js
   │
   ▼
┌─────────────────────────────────┐
│  Phase 1: Lexical Analysis      │  lexer.l  →  token stream
└─────────────────────────────────┘
   │
   ▼
┌─────────────────────────────────┐
│  Phase 2: Syntax Analysis       │  parser.y  →  AST (in memory)
└─────────────────────────────────┘
   │
   ▼
┌─────────────────────────────────┐
│  Phase 3: Semantic Analysis     │  infer types, build symbol table
└─────────────────────────────────┘
   │
   ▼
┌─────────────────────────────────┐
│  Phase 4: Code Generation       │  walk AST  →  output.ts
└─────────────────────────────────┘
```

---

## Build & Run

```bash
# Install dependencies (Ubuntu/WSL)
sudo apt install build-essential flex bison

# Build
make

# Run all inputs, save outputs and traces
make run

# Run a single file
./js2ts trace.txt input.js < input.js

# Clean
make clean
```

---

## Supported JS Constructs

| Construct | Example |
|---|---|
| Async function | `async function f(a, b) { }` |
| Parameters | `(users, userData)` |
| `const` / `let` | `const x = expr` |
| `if` / `else` | `if (cond) { } else { }` |
| `throw` | `throw new Error("msg")` |
| `return` | `return value` |
| Object literal | `{ id: 1, name: "x" }` |
| Array literal | `[]` / `[{ ... }]` |
| Arrow function | `x => expr` / `x => { }` |
| Method call | `arr.find(x => ...)` |
| Chained calls | `.then(...).catch(...)` |
| Binary ops | `===`, `!==`, `<`, `>`, `+`, `-`, `*`, `/`, `\|\|`, `&&` |
| Unary | `!x` |
| Compound assign | `x -= y` |

---

## What Makes This a Real Compiler

The old version matched hardcoded token sequences and printed pre-written TypeScript templates.
This version:

1. **Parses structurally** — grammar rules describe JS constructs, not specific token sequences
2. **Builds an AST** — every parsed construct becomes a node in memory
3. **Infers types** — semantic pass walks the AST and derives types from what it sees
4. **Generates from the AST** — code generation walks the tree and emits TypeScript, not a template

---

## Error Handling

- Syntax errors are collected with line numbers (parser does not stop at first error)
- After parsing, if any errors exist, a formatted table is printed and no output is produced
- `%define parse.error verbose` gives descriptive messages: `unexpected ASSIGN, expecting IDENTIFIER`
