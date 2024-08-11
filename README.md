# statimc: phase zero

| Operator | Precedence | Usage 
|----------|------------|------
| `=` `+=`, `-=`, `*=`, `/=` | 1 | Assignment
| `&&`, `\|\|`, `^^` | 2 | Logical Comparison
| `&`, `\|`, `^` | 3 | Bitwise Comparison
| `==`, `!=` | 4 | Equality Comparison
| `<`, `<=`, `>`, `>=` | 5 | Inequalities
| `<<`, `>>` | 6 | Bitwise Shift
| `+`, `-` | 7 | Additive Ops
| `*`, `/` | 8 | Multiplicative Ops
| `!`, `#`, `@`, `...` | 9 | Unary (not, hash, ref, range)
| `f()`, `.` | 10 | Calls, Member Access
| `a++`, `a--` | 11 | Increment, Decrement


### Types

`bool` boolean value (0, 1)

`i32` 32-bit signed integer.

`u32` 32-bit unsigned integer.

`i64` 64-bit signed integer.

`u64` 64-bit unsigned integer.

`float` floating point value.

`float64` 64-bit floating point value.

`char` singular character.

`byte` singular byte.

`str` character sequence.

`bstr` byte sequence.

`#Type` rune type.

### Variables

Mutable assignments
```rs
// initialization
let x: i32;

// declaration
x = 5;

// initialization and declaration
let x: i32 = 5;
```

Fixed assignment
> Must be declared the same line it is initialized.
```rs
fix y: i32 = 6;
```

Alias assignments
```rs
let y: i32 = 5;
bind x, y;
```

### Control Flow

If/else statements,

```c
if expression {
  ...
}
else if expression {
  ...
}
else {
  ...
}
```

Switch statements,

```c
match expression {
  x => ...;
  y => ...;
  z => ...;
}
```

### Looping instructions

Until loops
> Executes body until expression is met.
```c
until expression {
  ...
}
```

### User-defined Types

Structs
```rs
struct A {
  a: i64;
  b: u64;
}
```

Define an abstract
```rs
abstract Fun {
  fn run() -> i32;
}
```

Implement an abstract to a struct,
```rs
impl Fun for A {
  fn run() -> i32 { 
    ...
  }
}
```

Enumerations
```rs
enum Token {
  Plus,
  Minus,
  ...
}
```

Unions
```rs
union Optional {
  Token,
  null
}
```

### Packages

Define a package file using `pkg FILE`. Import it elsewhere using the `use FILE`
keyword. Keep definitions private using protection,

```rs
protect fn secret_function() -> void {
  ...
}
```
