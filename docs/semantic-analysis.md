# Semantic Analysis (SDD + SDT with Explanations)

**SDD (Syntax Directed Definition)** tells **what attributes are computed** for each grammar rule.
**SDT (Syntax Directed Translation)** tells **what action is executed during parsing**.

Use this format in viva/written exam.

# Attribute Legend

| Attribute | Meaning                |
| --------- | ---------------------- |
| `type`    | datatype               |
| `val`     | computed numeric value |
| `lexval`  | actual token text      |
| `place`   | variable/temp name     |
| `code`    | generated code         |
| `scope`   | scope name             |



---

# 1. Program Rule

## Production

```text id="o6z0zy"
program → stmt_list
```

## SDD

```text id="y6jngw"
program.code = stmt_list.code
```

### Explanation

The complete program consists of all statements, so generated code of program is taken from statement list.

## SDT

```text id="x6v6t0"
{ print("Compilation Successful"); }
```

### Explanation

After full parsing succeeds, compiler confirms valid program.

---

# 2. Statement List

## Production

```text id="g0a66o"
stmt_list → stmt_list stmt
```

## SDD

```text id="6x4v1r"
stmt_list.code = stmt_list1.code || stmt.code
```

### Explanation

Existing statement code is combined with new statement code.

## SDT

```text id="qux30u"
{ append statement output }
```

### Explanation

Each new parsed statement is added sequentially.

---

# 3. Async Function Declaration

## Production

```text id="n9o6q3"
async_func_decl →
async function IDENTIFIER(param_list){func_body}
```

## SDD

```text id="j4a87f"
IDENTIFIER.lexval = function name
async_func_decl.type = function
async_func_decl.returnType = Promise<T>
```

### Explanation

The parser stores function name, recognizes it as function type, and async functions return Promise.

## SDT

```text id="db3c0h"
{ sym_add(name, signature, global); }
```

### Explanation

Function is inserted into symbol table so later calls can be checked.

---

# 4. Parameter List

## Production

```text id="3fy1lh"
param_list → IDENTIFIER , IDENTIFIER
```

## SDD

```text id="epn2a5"
count = 2
param names = identifiers
```

### Explanation

Compiler records number of parameters and their names.

## SDT

```text id="krv9pr"
{ store parameters in current scope }
```

### Explanation

Parameters behave as local variables inside function body.

---

# 5. Variable Declaration

## Production

```text id="r9dn8f"
var_decl → let IDENTIFIER = [ ]
```

## SDD

```text id="zqjlwm"
IDENTIFIER.type = array
```

### Explanation

Empty brackets indicate array initialization.

## SDT

```text id="l6n2uv"
{ add variable to symbol table }
```

### Explanation

Variable must be stored for later usage.

---

# 6. Constant Declaration

## Production

```text id="36r6je"
const_decl → const IDENTIFIER = expr
```

## SDD

```text id="0kw45k"
IDENTIFIER.type = expr.type
IDENTIFIER.val = expr.val
```

### Explanation

Constant gets type and value from assigned expression.

## SDT

```text id="0qwr1p"
{ insert constant in table }
```

### Explanation

Compiler stores immutable variable entry.

---

# 7. Numeric Constant Expression

## Production

```text id="l5r0zi"
const_decl → const MAX_SIZE = 5 * 1024 * 1024
```

## SDD

```text id="rtg11d"
type = number
val = multiplication result
```

### Explanation

Arithmetic expression is evaluated during compile time if literals are known.

## SDT

```text id="9pq7ri"
{ store MAX_SIZE = 5242880 }
```

### Explanation

This is constant folding optimization.

---

# 8. If Statement

## Production

```text id="e4pg1x"
if_stmt → if(if_cond){throw_stmt}
```

## SDD

```text id="z55m4o"
if_cond.type = boolean
```

### Explanation

Condition must evaluate to true/false.

## SDT

```text id="vs3wd7"
{ validate condition type }
```

### Explanation

Rejects invalid conditions if unsupported type found.

---

# 9. Condition Rule

## Production

```text id="9u6q7v"
if_cond → !fileData.name || !fileData.size
```

## SDD

```text id="dn2pzi"
if_cond.type = boolean
```

### Explanation

Logical NOT and OR operations always produce boolean result.

---

# 10. Throw Statement

## Production

```text id="f12af9"
throw_stmt → throw new Error("msg")
```

## SDD

```text id="k3ahmv"
STRING.type = string
```

### Explanation

Error constructor receives message text.

## SDT

```text id="0n9d0r"
{ emit throw instruction }
```

### Explanation

Generated output preserves runtime exception behavior.

---

# 11. Number Expression

## Production

```text id="twqjqa"
expr → NUMBER
```

## SDD

```text id="tpr5m0"
expr.type = number
expr.val = token value
```

### Explanation

Numeric token becomes numeric expression.

---

# 12. String Expression

## Production

```text id="08c4q8"
expr → STRING
```

## SDD

```text id="1g0aqf"
expr.type = string
```

### Explanation

String literal directly has string type.

---

# 13. Identifier Expression

## Production

```text id="ebw5vr"
expr → IDENTIFIER
```

## SDD

```text id="pqv7x6"
expr.type = lookup(id).type
```

### Explanation

Compiler checks symbol table to know declared type.

## SDT

```text id="d2m1tm"
{ verify identifier exists }
```

### Explanation

Prevents use of undeclared variables.

---

# 14. Arithmetic Expression

## Production

```text id="8m6b6t"
expr → NUMBER * NUMBER
```

## SDD

```text id="2jzq0k"
expr.type = number
expr.val = n1 * n2
```

### Explanation

Multiplication of numbers gives numeric result.

---

# 15. Comparison Expression

## Production

```text id="hl23n4"
expr → f.name === fileData.name
```

## SDD

```text id="32h75j"
expr.type = boolean
```

### Explanation

Equality comparison returns true or false.

---

# 16. Function Call

## Production

```text id="7stx5e"
expr → files.push(newFile)
```

## SDD

```text id="jbrh7h"
expr.type = returnType(method)
```

### Explanation

Method call result depends on called function.

## SDT

```text id="cvbr3f"
{ validate arguments count/types }
```

### Explanation

Ensures proper function usage.

---

# 17. Arrow Function

## Production

```text id="7v7g1g"
arrow_func → f => expr
```

## SDD

```text id="2m3wr9"
param = f
returnType = expr.type
```

### Explanation

Arrow function accepts parameter and returns expression type.

## SDT

```text id="2z4h9s"
{ create local lambda scope }
```

### Explanation

Parameter should exist only inside arrow function body.

---

# 18. Object Literal

## Production

```text id="ww6wvw"
object_literal → { prop_list }
```

## SDD

```text id="5r0u0g"
type = record/object
```

### Explanation

Set of named fields forms object structure.

---

# 19. Property Rule

## Production

```text id="2jmx2w"
prop → name : expr
```

## SDD

```text id="6m5n6z"
prop.name = identifier
prop.type = expr.type
```

### Explanation

Each object property stores key and value type.

## SDT

```text id="g8afg7"
{ save property metadata }
```

### Explanation

Used later to generate TypeScript interface/type.

---

# 20. Return Statement

## Production

```text id="2gx0xq"
return_stmt → return newFile
```

## SDD

```text id="3xj4xq"
return.type = newFile.type
```

### Explanation

Returned value determines statement type.

## SDT

```text id="3ghh3u"
{ compare with function return type }
```

### Explanation

Ensures function returns correct type.

---

# Viva One-Liner

**SDD computes attributes. SDT performs semantic actions during parsing.**
