# General Documentation

This project is a **mini JS → TS compiler** built using **Flex** and **Bison**. It processes a subset of JavaScript syntax and generates TypeScript output.



---

# Project Phases

1. **Lexical Analysis** – Tokenizes source code using Flex
2. **Syntax Analysis** – Parses grammar using Bison (LALR(1))
3. **Semantic Analysis** – Builds symbol/type information
4. **Code Generation** – Produces TypeScript file

---

# Main Files

| File         | Purpose                    |
| ------------ | -------------------------- |
| `lexer.l`    | Scanner rules (tokens)     |
| `parser.y`   | Grammar + semantic actions |
| `symtable.h` | Symbol table declarations  |
| `symtable.c` | Symbol table logic         |
| `input.js`   | Source input program       |

---

# Auto-Generated Files

| File                     | Created By |
| ------------------------ | ---------- |
| `lex.yy.c`               | Flex       |
| `parser.tab.c`           | Bison      |
| `parser.tab.h`           | Bison      |
| `compiler.exe` / `a.out` | GCC        |

---

# Output File

| File        | Purpose                   |
| ----------- | ------------------------- |
| `output.ts` | Generated TypeScript code |

---

# Supported Constructs

* `async function`
* `let`, `const`
* `if`
* `return`
* `throw new Error(...)`
* Arrays / Objects
* Function calls
* Arrow functions
* Comparisons (`===`, `<`, `>`)
* Arithmetic (`*`, `-=`)

---

# Build Commands

```bash
bison -d parser.y
flex lexer.l
gcc lex.yy.c parser.tab.c symtable.c -o compiler
./compiler < input.js
```

---

# Flow

```text
input.js
 ↓
Lexer
 ↓
Parser
 ↓
Semantic Checks
 ↓
Code Generator
 ↓
output.ts
```

---

# Note

Separate documentation can now be created for:

1. Lexical Phase
2. Syntax Phase
3. Semantic Phase
4. Code Generation
