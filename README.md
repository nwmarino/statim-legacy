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

### Primitive Types

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

Mutable local assignments using `let`:
```rs
// initialization
let x: i32;

// declaration
x = 5;

// initialization and declaration
let x: i32 = 5;
```

Immutable local assignments using `fix`:
> Must be declared the same line it is initialized.
```rs
fix y: i32 = 6;
```

Binding statements with `bind`:
> Binds a new variable to an existing one for its lifetime.
```rs
let y: i32 = 5;
bind x, y;
```

### Control Flow

If then/else then statements using `if`, `else`:

```rs
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

Pattern matching using `match`:

```rs
match expression {
  x => ...,
  y => ...,
  z => ...,
  _ => ...
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
```rs
struct Dog {
  name: str;
}
```

Define an abstract interface using `abstract`:
```rs
abstract MakeNoise {
  fn woof() -> str;
}
```

Implement all interface methods to a struct using `impl`:
```rs
impl MakeNoise for Dog {
  fn woof() -> str { 
    ...
  }
}
```

Check that a `struct` type implements an interface using `impls`:
```rs
Dog impls MakeNoise == true
```

Declare an enumerated type using `enum`:
```rs
enum Token {
  Plus,
  Minus,
  ...
}
```

### Packages

Import another source file using `pkg FILENAME`.

Keep definitions private using the `protect` keyword:
```rs
protect fn secret_function() -> void {
  ...
}
```
