# statimc: legacy
***

# Supported

### Types

`i32` 32-bit signed integer type.

### Functions 

`fn main() -> i32 {}` main entry point

`fn 'identifier' ( args ) -> rt_type` function prototype

# Future

### Types

`u32` 32-bit unsigned integer type.

`char` single character type.

`fp` floating point value type.

`double` double value type.

`bool` boolean value type.

`str` string value type.

`*Type` rune type.

### Variables

Assignments (mutable variables),
```rs
// initialization
let x: i32;

// declaration
x = 5;

// initialization and declaration
let x: i32 = 5;
```

Assignments (immutable variables),
> Must be declared the same line it is initialized.
```rs
fix y: i32 = 6;
```

Alias assignments using the `bind` keyword, 
> The following example binds a variable `x` to the variable `y`.
```rs
let y: i32 = 5;
bind x, y;
```

### Unary Operators

Negation operator,
```rs
let x: bool = true;
let not_x: bool = !x;
```

Increment operator,
```rs
let x: i32 = 0;
x++;
```

Decrement operator,
```rs
let x: i32 = 0;
x--;
```

Dereference (retrieve value),
```rs
let x: *i32 = 0;
let y: i32 = *x;
```

Reference (retrieve address),
```rs
let x: *i32 = 0;
let y = &x;
``` 

### Binary Operators

Add operator,
```rs
let x: i32 = 1 + 5;
```

Subtraction operator,
```rs
let x: i32 = 1 - 5;
```

Multiplication operator,
```rs
let x: i32 = 1 * 5;
```

Division operator,
```rs
let x: i32 = 4 / 2;
```

Power operator,
```rs
let x: i32 = 1 ** 2;
```

AND operator,
```rs
let is_true: bool = true && true;
let is_false: bool = true && false;
```

OR operator,
```rs
let is_true: bool = true || false;
let is_false: bool = false || false;
```

XOR operator,
```rs
let is_true: bool = true ^ false;
let is_false: bool = true ^ true;
let is_false_two: bool = false ^ false;
```

Inequality operators,
```rs
let is_true: bool = 1 < 2;
let is_false: bool = 1 > 2;

let is_true_two: bool = 1 <= 2;
let is_false_two: bool = 1 >= 2;
```

Value comparisons,
```rs
let x: i32 = 1;
let y: i32 = 2;
let z: i32 = 1;

let is_true: bool = x == z;
let is_false: bool = x == y;
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
switch expression {
  case expression:
    ...
    break;
  case expression:
    ...
    break;
  default:
    ...
    break;
}
```

### Loops

While loops,

```c
while (expression) {
  ...
}
```

For loops,

```c
for (start, stop, step) {
  ...
}
```

Special "until" loops,
> Until loops stop all execution once the expression is met. The
> loop body must possess some flow to statements which can meet
> the condition.

```c
until (expression) {
  ...
}
```
