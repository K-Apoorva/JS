# JS → TS Compiler (Lex + Bison)
### Compiler Design Project — 3 Phases

---

## Project Structure

```
js2ts/
├── lexer.l       ← Phase 1: Lexical Analysis (Flex)
├── parser.y      ← Phase 2 & 3: Syntax + Semantic Analysis (Bison)
├── input.js      ← Input JavaScript source
├── Makefile      ← Build system
└── README.md
```

---

## Phases

### Phase 1 — Lexical Analysis (`lexer.l`)
Flex tokenizes the JS character stream into tokens:
- Keywords: `async`, `function`, `const`, `let`, `if`, `throw`, `return`, `new`
- Identifiers, String/Number literals
- Operators: `===`, `||`, `=>`, `!`
- Punctuation: `( ) { } [ ] , ; : .`

### Phase 2 — Syntax Analysis (`parser.y`)
Bison validates grammar and builds an implicit AST:
- `AsyncFunction → async function Identifier ( Params ) Block`
- `IfStatement → if ( Expr ) { throw ... }`
- `ArrowFunction → Identifier => Expr`
- `ObjectLiteral → { PropertyList }`
- `VariableDeclaration → const Identifier = Expr`

### Phase 3 — Semantic Analysis (`parser.y`)
- Symbol Table: tracks identifier names, inferred types, and scope
- Type Inference: infers `User`, `UserInput`, `User[]`, `Promise<User>`
- Type Checking: validates call-site argument types (string vs number/boolean)
- Code Generation: emits fully typed TypeScript output

---

## Build & Run

```bash
make        # compile
make clean  # clean the generated files
make run    # run on input.js
make clean  # remove generated files
```

---

## Sample Output

```
[Parser] ✔ AsyncFunction: registerUser
[Parser] ✔ IfStatement (validation guard)
[Parser] ✔ ArrowFunction: u => ...
[Parser] ✔ MethodCall: users.find(ArrowFunction)
[Semantic] Property 'isActive' : boolean
...

SYMBOL TABLE:
users        → User[]           (param)
userData     → UserInput        (param)
newUser      → User             (local)
registerUser → async function(User[], UserInput): Promise<User>  (global)

Type Checker:
  ✓ email : string
  ✓ password : string

Generated TypeScript:
  type User = { id: number; email: string; ... }
  async function registerUser(users: User[], userData: UserInput): Promise<User>
```
