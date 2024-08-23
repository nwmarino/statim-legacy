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

AST BUILD PASS SYMBOL REFERENCES:

expr.cpp

`parse_identifier()`
- Check if an identifier is an existing variable/constant.
`parse_function_call()`
- Check if the function exists.

decl.cpp

`parse_definition()`
- Adds the function to the symbol table as a `function` by its prototype name.
`parse_struct()`
- Adds the struct to the symbol table as a `ty` by its name.
`parse_abstract()`
- Adds the abstract to the symbol table as a `interface` by its name.
`parse_immut_decl()`
- Adds the variable to the symbol table as a `constant` by its name and identifier metadata.
`parse_mut_decl()`
- Adds the variable to the symbol table as a `variable` by its name and identifier metadata.

Pass 1: Finish name resolution, typing

Prototypes
- If is "main", check that return type is void.
- Otherwise, check that a valid return type is given.
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
- Add methods and implement to struct.
- Check the implemented method(s) exist in the abstract.
- Check the implemented method(s) do not conflict with existing struct methods.

Packages
- Check multiple packages of the same name do not exist.
- Check a package is not imported more than once.
