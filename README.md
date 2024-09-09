
# statimc: phase zero

### Operations

| Operator | Precedence | Usage 
|----------|------------|------
| `=` `+=`, `-=`, `*=`, `/=` | 1 | Assignment
| `&&`, `\|\|` | 2 | Logical Comparison
| `==`, `!=` | 3 | Equality Comparison
| `<`, `<=`, `>`, `>=` | 4 | Inequalities
| `+`, `-` | 5 | Additive Ops
| `*`, `/` | 6 | Multiplicative Ops
| `!`, `#`, `@` | 7 | Unary (not, hash, ref)
| `f()`, `.` | 8 | Calls, Member Access

### Types

| Symbol | Type | Example Literal
|--------|------|----------------
| `bool` | boolean | `true`, `false`
| `uint` | unsigned integer (32-bit) | `0`, `1`, `2147483648`
| `i32`  | signed integer (32-bit) | `-1`, `0`, `2147483648`
| `i64`  | signed integer (64-bit) | `9.223372e+18`
| `float` | floating point (32/64-bit) | `0.25`, `3.14`
| `char` | base character | `'s'`, `'t'`, `'a'`, `'T'`, `'i'`, `'m'`
| `str` | character sequence | `"staTim"` 
| `#Type` | rune (ptr) | ...
| `Array[T]` | fixed array of T | `[1, 2, 3]`, `['a', 'b', 'c']`

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
Define common behaviours using `trait`:
```
trait CanSwim {
  fn swim() -> void;
}
```
Implement methods to a struct using `impl`:
```
impl Shark {
  fn bite() -> bool {
    ...
  }
}
```
Implement common behaviour to a struct using `impl`:
```
impl CanSwim for Shark {
  fn swim() -> str { 
    ...
  }
}
```
Declare an enumerated type using `enum`:
```
enum Operation {
  Plus,
  Minus,
  ...
}
```
Declare a variable of an `enum` type:
```
let op: Operation = Operation::Plus;
```

### Runes

Declare a rune with the `#` hash unary, and reference its position with `@` ref unary:
```
struct Tree {
  child: #Tree
}

...

let mut Leaf: #Tree = Tree {
  child: null
}

let mut Root: #Tree = Tree {
  child: @Leaf
}
```

### Arrays

Define an array using `Array[T; n]`:
```
let mut numbers: Array[i32; 1];
```
Read/write elements at an index `i`:
```
numbers[0] = 0;
let x: i32 = numbers[0];
```

### Packages

Import another source file using `pkg`:
```
pkg file;
...
```
Import packages from other programs:
```
pkg prog::file;
...
```
Keep declarations private to their parent scope using the `priv` keyword:
```
priv fn secret_function() -> bool {
  ...
}
```
