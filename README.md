# JS → TS Compiler (Lex + Bison)
### Compiler Design Project — 4 Phases

---

## Project Structure

```
js2ts/
├── lexer.l              ← Phase 1: Lexical Analysis (Flex)
├── parser.y             ← Phase 2–4: Syntax + Semantic + CodeGen (Bison + C)
├── ast.h / ast.c        ← AST node definitions
├── input.js             ← Sample input 1 (registerUser)
├── input2.js            ← Sample input 2 (uploadFile)
├── input3.js            ← Sample input 3 (placeOrder)
├── input_error.js       ← Sample input with syntax errors
├── Makefile             ← Build system
├── README.md
└── docs/
    ├── js2ts.md                  ← Project overview
    ├── lexical-analysis.md       ← Phase 1 documentation
    ├── syntax-analysis.md        ← Phase 2 documentation
    ├── semantic-analysis.md      ← Phase 3 documentation (SDD + SDT)
    └── target-code-generation.md ← Phase 4 documentation
```

---

## Phases

### Phase 1 — Lexical Analysis (`lexer.l`)
Flex tokenises the JS character stream. Recognises:
- Keywords: `async`, `function`, `const`, `let`, `if`, `else`, `throw`, `return`, `new`, `true`, `false`, `null`
- Identifiers, string/number literals
- Operators: `===`, `!==`, `<=`, `>=`, `||`, `&&`, `=>`, `!`, `+=`, `-=`, `*=`
- Arithmetic: `+`, `-`, `*`, `/`, `%`
- Punctuation: `( ) { } [ ] , ; : .`
- Skips whitespace, `//` comments, `/* */` block comments

### Phase 2 — Syntax Analysis (`parser.y`)
Bison LALR(1) parser. General grammar — not hardcoded patterns:
- Full expression precedence hierarchy (assign → or → and → eq → rel → add → mul → unary → postfix → primary)
- Builds an **AST in memory** (tagged union of `Node` structs)
- Error recovery: collects all syntax errors with line numbers, reports them all, aborts if any found
- `%define parse.error verbose` for descriptive error messages

### Phase 3 — Semantic Analysis (`parser.y`)
Post-parse AST walk:
- **Type inference**: derives object type shapes from `const newX = { ... }` literals
- **Param type inference**: detects array params from `.find()`/`.push()` usage, derives element type from param name
- **Input type inference**: derives input object type name from param name (`userData` → `UserInput`)
- **Symbol table**: scope-aware, tracks all variables and their inferred types
- **Semantic error checks**: missing params, unresolvable return type, unknown property types

### Phase 4 — Code Generation (`parser.y`)
AST walk that emits TypeScript:
- Type aliases from the type registry
- Typed function signatures from the symbol table
- All statements and expressions recursively emitted
- Arrow function params annotated with element type in `.find()`/`.filter()` calls
- Parse trace written to separate file per input

---

## Build & Run

```bash
# Install (Ubuntu/WSL)
sudo apt install build-essential flex bison

make          # compile
make run      # run all 3 inputs + error test, save outputs and traces
make clean    # remove generated files
```

Run a single file:
```bash
./js2ts trace.txt input.js < input.js
cat output.ts
```

---

## Sample Output

**Symbol Table (input.js):**
```
║ newUser      ║ User                              ║ registerUser ║
║ users        ║ User[]                            ║ registerUser ║
║ userData     ║ UserInput                         ║ registerUser ║
║ registerUser ║ async function(...): Promise<User> ║ global      ║
```

**Generated TypeScript (output1.ts):**
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
    ...
}

let users: User[] = [];
```

**Error output (input_error.js):**
```
║ 3    ║ syntax error, unexpected LBRACE, expecting RBRACE or IDENTIFIER ║
║ 7    ║ syntax error, unexpected ASSIGN, expecting IDENTIFIER           ║

❌ Compilation aborted — 2 error(s) found. No output produced.
```
