# Phase 1 — Lexical Analysis

## Role

The lexer is the first phase of the compiler. It reads the raw JavaScript source character by character and converts it into a flat stream of **tokens** — the smallest meaningful units of the language. The parser never sees raw characters; it only sees tokens.

Implemented in `lexer.l` using **Flex**.

---

## How Flex Works

Flex reads pattern rules of the form:

```
PATTERN   { action }
```

When the input matches a pattern, Flex executes the action — typically returning a token type to the parser and optionally storing the token's text in `yylval`.

---

## Token Categories

### Keywords

Each keyword has its own token constant so the parser can distinguish it from an identifier.

| Input | Token |
|---|---|
| `async` | `ASYNC` |
| `function` | `FUNCTION` |
| `const` | `CONST` |
| `let` | `LET` |
| `if` | `IF` |
| `else` | `ELSE` |
| `return` | `RETURN` |
| `throw` | `THROW` |
| `new` | `NEW` |
| `true` | `TRUE_LIT` |
| `false` | `FALSE_LIT` |
| `null` | `NULL_LIT` |

### Operators

| Input | Token | Notes |
|---|---|---|
| `===` | `EQ` | strict equality |
| `!==` | `NEQ` | strict inequality |
| `<=` | `LE` | |
| `>=` | `GE` | |
| `<` | `LT` | |
| `>` | `GT` | |
| `\|\|` | `OR` | |
| `&&` | `AND` | |
| `=>` | `ARROW` | arrow function |
| `=` | `ASSIGN` | assignment |
| `-=` `+=` `*=` | `ASSIGN_OP` | compound assignment |
| `!` | `NOT` | |
| `+` `-` | `ADDOP` | stores operator text in `yylval.str` |
| `*` `/` `%` | `MULOP` | stores operator text in `yylval.str` |

### Punctuation

| Input | Token |
|---|---|
| `.` | `DOT` |
| `,` | `COMMA` |
| `;` | `SEMICOLON` |
| `:` | `COLON` |
| `(` `)` | `LPAREN` `RPAREN` |
| `{` `}` | `LBRACE` `RBRACE` |
| `[` `]` | `LBRACKET` `RBRACKET` |

### Literals

| Pattern | Token | Value stored |
|---|---|---|
| `"..."` (any chars, escaped ok) | `STRING_LIT` | full string including quotes in `yylval.str` |
| `[0-9]+(\.[0-9]+)?` | `NUMBER_LIT` | numeric text in `yylval.str` |

### Identifiers

```
[a-zA-Z_$][a-zA-Z0-9_$]*
```

Returns `IDENTIFIER`, stores name in `yylval.str`.

---

## Ignored Input

```
[ \t\r\n]+        whitespace
//[^\n]*          single-line comments
/* ... */         block comments
.                 any unrecognised character
```

These are silently consumed — the parser never sees them.

---

## Concrete Example

Input:
```javascript
const newUser = { id: Date.now(), isActive: true };
```

Token stream produced:
```
CONST  IDENTIFIER("newUser")  ASSIGN  LBRACE
  IDENTIFIER("id")  COLON  IDENTIFIER("Date")  DOT  IDENTIFIER("now")  LPAREN  RPAREN
  COMMA
  IDENTIFIER("isActive")  COLON  TRUE_LIT
RBRACE  SEMICOLON
```

---

## Output

Running `flex lexer.l` generates `lex.yy.c` — the C implementation of the scanner. This is compiled together with the parser.

---

## Key Design Points

- Longer patterns take priority over shorter ones (e.g. `===` matches before `=`)
- Keywords are listed before the identifier rule so `async` returns `ASYNC`, not `IDENTIFIER`
- `yylval.str` is populated with `strdup(yytext)` for tokens whose text matters downstream (identifiers, literals, operators used in expressions)
- `yylineno` is automatically tracked via `%option yylineno` — used for error reporting
