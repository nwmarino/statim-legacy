program:        globalScope

globalScope:    declarations

scope:          `{` globalScope `}`
                | `{` `}`

statements:     `fix` variable `:` type `=` expression                              # constant declaration
                | `let` variable `:` type                                           # variable initialization
                | `let` variable `:` type `=` expression                            # variable initialization and declaration
                | `return` expression                                               # return expression from function
                | `if` expression scope                                             # execute scope if expression is true
                | `if` expression scope `else if` expression scope ...
                | `if` expression scope `else` scope
                | `if` expression scope `else if` expression scope `else` scope 
                | `while` expression scope                                          # loop while expression is true
                | `until` expression scope                                          # loop until expression is met
                | `break`                                                           # break statement from loop
                | `match`                                                           # case matching

declarations:   `fn` identifier `(` `)` `->` type scope                             # function
                | `fn` identifier `(` parameters `)` `->` type scope                # function with parameters

expressions:    integer
                | expression `+` expression                                         # addition
                | expression `-` expression                                         # subtraction
                | expression `*` expression                                         # multiplication
                | expression `/` expression                                         # division
                | expression `+` `+`                                                # unary increment
                | expression `-` `-`                                                # unary decrement
                | expression `+` `=` expression                                     # add by
                | expression `-` `=` expression                                     # subtract add by
                | expression `*` `=` expression                                     # multiply add by
                | expression `/` `=` expression                                     # divide add by
                | `true`                                                            # boolean true
                | `false`                                                           # boolean false
                | `!` expression                                                    # negate expression
                | expression `&&` expression                                        # boolean and operator
                | expression `||` expression                                        # boolean or operator
                | expression `^^` expression                                        # boolean xor operator
                | expression `==` expression                                        # comparison operator
                | expression `!=` expression                                        # compare not equals
                | expression `<` expression                                         # less than inequality
                | expression `<=` expression                                        # less than eq inequality
                | expression `>` expression                                         # greater than inequality
                | expression `>=` expression                                        # greater than eq inequality
                | char
                | byte 
                | variable

parameters:     type identifier
                | parameters `,` parameters

type:           `bool`
                | `i32`
                | `u32`
                | `i64`
                | `u64`
                | `char`
                | `byte`
                | `float`
                | `struct`
