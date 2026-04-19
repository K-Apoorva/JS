# Code Generation Phase

The **Code Generation** phase is the final compiler stage. After lexical, syntax, and semantic analysis succeed, the compiler translates the validated JavaScript subset into **TypeScript**.

In your project, this is handled by:

```c id="1c5kz8"
emit_typescript();
```

which writes the generated code into:

```text id="1x1w8g"
output.ts
```



---

# Main Purpose

Convert parsed + semantically validated source code into typed TypeScript with:

* Type aliases
* Typed variables
* Typed function parameters
* Promise return types
* Preserved program logic

---

# Main Output File

| File        | Purpose                      |
| ----------- | ---------------------------- |
| `output.ts` | Generated TypeScript program |

---

# How Generation Works

The parser sets:

```c id="j0s3zt"
func_mode
```

Depending on detected function:

| Mode | Function         |
| ---- | ---------------- |
| `1`  | `registerUser()` |
| `2`  | `uploadFile()`   |
| `3`  | `placeOrder()`   |

Then `emit_typescript()` writes the matching TypeScript template.

---

# Example: uploadFile Mode

If parser detects:

```javascript id="9m6n9g"
async function uploadFile(files, fileData)
```

Generator emits:

```typescript id="v6f8ne"
type FileRecord = {
  id: number;
  name: string;
  size: number;
  uploadedAt: string;
};
```

and:

```typescript id="e7j2n6"
async function uploadFile(
   files: FileRecord[],
   fileData: FileInput
): Promise<FileRecord>
```

---

# Code Generation Steps

# 1. Emit Type Definitions

Creates TypeScript types:

```typescript id="9m4r0p"
type FileRecord = {...}
type FileInput = {...}
```

### Why

JavaScript is loosely typed, TypeScript requires explicit structure.

---

# 2. Emit Function Signature

Generated:

```typescript id="7t7j6j"
async function uploadFile(
 files: FileRecord[],
 fileData: FileInput
): Promise<FileRecord>
```

### Why

Semantic analysis already determined parameter and return types.

---

# 3. Emit Statements

Compiler reproduces logic:

```typescript id="m4k8xa"
if (!fileData.name || !fileData.size)
throw new Error(...)
```

```typescript id="v5h2zt"
const MAX_SIZE: number = 5 * 1024 * 1024;
```

```typescript id="g0w8fa"
return newFile;
```

---

# 4. Emit Variable Declarations

Generated:

```typescript id="y7g5lz"
let files: FileRecord[] = [];
```

### Why

Array type inferred during semantic phase.

---

# Internal Generation Method

Your code uses:

```c id="u8d2br"
FILE *f = fopen("output.ts","w");
fprintf(...)
fclose(f);
```

### Meaning

* Opens file
* Writes TypeScript line by line
* Saves output

---

# Why Separate Code Generation Phase?

Earlier phases only verify correctness.

This phase creates usable target code.

```text id="e0z7p7"
Lexer      → tokens
Parser     → structure
Semantic   → meaning
CodeGen    → final program
```

---

# Sample Transformation

## Input JS

```javascript id="1m0l3z"
let files = [];
```

## Output TS

```typescript id="7x4j3r"
let files: FileRecord[] = [];
```

---

## Input JS

```javascript id="n2r5o8"
const MAX_SIZE = 5 * 1024 * 1024;
```

## Output TS

```typescript id="9d4l0q"
const MAX_SIZE: number = 5 * 1024 * 1024;
```

---

# Success Message

After generation:

```text id="7x9n2d"
[CodeGen] ✔ TypeScript written to output.ts
Compilation successful.
```

---

# Exam Keywords

Mention these:

* Target code generation
* Source-to-source translation
* Typed output
* Template-based generation
* File emission using `fprintf()`

---

# Summary

The code generation phase converts validated JavaScript input into structured **TypeScript output** by emitting types, typed functions, declarations, and original logic into `output.ts`.
