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

### Primitive Types

`bool` boolean value (false, true)

`unsigned` 64-bit unsigned integer.

`u8` 

`i32` 32-bit signed integer.

`i64` 64-bit signed integer.

`float` floating point value.

`char` singular character.

`byte` singular byte.

`str` character sequence.

`bstr` byte sequence.

`#Type` rune type.
> For example, a rune `A` struct: `let a: #A = A { ... };`

### Variables

Variable assignments using `let`, and mutable with `mut`:
```rs
let mut x: i32;
x = 5;

// immutable declaration
let y: i32 = 5;
```

- [x] Parsing
- [ ] Passed
- [ ] Lowering
- [ ] Codegen

Binding statements with `bind`:
> Binds a new variable to an existing one for its lifetime.
```rs
let y: i32 = 5;
bind x, y;
```

- [ ] Parsing
- [ ] Passed
- [ ] Lowering
- [ ] Codegen

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

- [x] Parsing
- [ ] Passed
- [ ] Lowering
- [ ] Codegen

Pattern matching using `match`:

```rs
match expression {
  x => ...,
  y => { ... },
  z => ...,
  _ => { ... }
}
```

- [x] Parsing
- [ ] Passed
- [ ] Lowering
- [ ] Codegen

### Looping instructions

Until loop statements:
> Executes body until expression is met.
```rs
until expression {
  ...
}
```

- [x] Parsing
- [ ] Passed
- [ ] Lowering
- [ ] Codegen

### User-defined Types

Define a type using `struct`:
```rs
struct Dog {
  name: str,
  breed: str,
}
```

- [x] Parsing
- [ ] Passed
- [ ] Lowering
- [ ] Codegen

Define common behaviours using `trait`:
```rs
trait MakeNoise {
  fn woof() -> str;
}
```

- [x] Parsing
- [ ] Passed
- [ ] Lowering
- [ ] Codegen

Implement all interface methods to a struct using `impl`:
```rs
impl MakeNoise for Dog {
  fn woof() -> str { 
    ...
  }
}
```

- [x] Parsing
- [ ] Passed
- [ ] Lowering
- [ ] Codegen

Check that a `struct` type implements an interface using `impls`:
```rs
Dog impls MakeNoise == true
```

- [ ] Parsing
- [ ] Lowering
- [ ] Codegen

Declare an enumerated type using `enum`:
```rs
enum Token {
  Plus,
  Minus,
  ...
}
```

- [x] Parsing
- [ ] Lowering
- [ ] Codegen

### Packages

Import another source file using `pkg NAME`.

Keep top-level declarations private to their package using the `priv` keyword:
```rs
priv fn secret_function() -> void {
  ...
}
```
