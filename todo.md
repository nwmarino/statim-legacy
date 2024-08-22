Parser

- [x] fixed variables
- [x] variables, just initialization
- [x] variables, init and declaration
- [x] return with expression
- [x] return w/o expression (implicit null)
- [x] parse function args in prototype
- [x] binary operations
- [x] function calls
- [x] until loops
- [x] control flow
- [x] match statements
- [x] import statements

Pass 1: Finish name resolution, typing

Prototypes
- If is "main", check that return type is void.
- Otherwise, check that a return type is given.
- Add function name to scope table.

Variables
- Check that the provided literal is of the same type.
- Check that the type exists in the scope.

Until loop
- Check that the expression is not a constant.
  - Check that the expression is not always false.
- Check that the expression can be evaluated to truthy/falsy.

- Check that any `continue`, `break` statements are within the scope of a loop.

If statements
- Check that the expression is not always false.

Match statement
- Check that a default _ case exists.
  - For boolean matches, only a true and false must exist.

Structs
- Check that the field type exists.

Abstract
- Check that return types exist.

Impl

Packages
- Check multiple packages of the same name do not exist.
- Check a package is not imported more than once.
