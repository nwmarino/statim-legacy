# noral

# Types

`int` Whole, numerical literals.

`float` Floating point value.

`bool` Boolean value.

`char` Singular UTF-8 value.

`str` Array of `char` objects.

# Basics

Function header.
```c
fn foo(arg1, arg2, ...) -> return_type {
  ...
}
```

Return statement.
```c
fn foo(arg1) -> return_type {
  return arg1;
}
```

Assignments (mutable variables)
```c
> initialization
let x: int;

> declaration
x = 5;

> initialization and declaration
let x: int = 5;
```

Assignments (immutable variables)
> Must be declared the same line it is initialized.
```c
fix y: int = 6;
```

Alias assignments using the `bind` keyword. 
> Must be declared the same line a bind is initialized.
```C
let x: int = 5;
bind y: int = 5;
```

Conditional operators `if`, `else`, `else if`:
```c
if x { ... }

else if y { ... }

else z { ... }
```

# Operators

Adding values.
```c
let x: int = 4;
let y: int = 2;

let z: int = x + y;

assert(z == 6);
```

Subtracting values.
```c
let z: int = x - y;

assert(z == 2);
```

Multiplying values.
```c
let z: int = x * y;

assert(z == 8);
```

Dividing values.
```c
let z: int = x / y;

assert(z == 2);
```

Values to a power.
```c
let z: int = x ** y;

assert(z == 16);
```

