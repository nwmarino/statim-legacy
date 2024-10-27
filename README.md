
# statim: p0

### Operations

| Operator | Precedence | Usage 
|----------|------------|------
| `=` `+=`, `-=`, `*=`, `/=` | 1 | Assignment
| `&&`, `\|\|` | 2 | Logical Comparison
| `==`, `!=` | 3 | Equality Comparison
| `<`, `<=`, `>`, `>=` | 4 | Inequalities
| `+`, `-` | 5 | Additive Ops
| `*`, `/` | 6 | Multiplicative Ops
| `!` | 7 | Unary (not)
| `f()`, `.` | 8 | Calls, Member Access

### Types

| Symbol | Type | Example Literal
|--------|------|----------------
| `bool` | boolean | `true`, `false`
| `i64`  | signed integer (64-bit) | `9.223372e+18`
| `char` | base character | `'s'`, `'t'`, `'a'`, `'T'`, `'i'`, `'m'`

### Variables

Variable assignments using `let`, and mutable with `mut`:
```
// mutable decl
let mut x: i32;
x = 5;

// immutable decl
let y: i32 = 5;
```

### Control Flow

If then/else then statements using `if`, `else`:

```rs
if expr {
  ...
}
else if expr {
  ...
}
else {
  ...
}
```

Pattern matching using `match`:

```rs
match expr {
  x => ...,
  y => { ... },
  z => ...,
  _ => { ... }
}
```

### Looping instructions

Until loop statements:
> Executes body until expression is met.
```rs
until expression {
  ...
}
```

### User-defined Types

Define a type using `struct`:
```
struct Shark {
  type: str,
  age: uint,
}
```
Construct defined types using initializer expressions:
```
let mut fishy: Shark = Shark {
  type: "Great White",
  age: 5,
}
```

### Packages

Import another source file using `pkg`:
```
pkg file;
...
```

Keep declarations private to their parent scope using the `priv` keyword:
```
priv fn secret_function() -> bool {
  ...
}
```
