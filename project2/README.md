
## required types of error
Type 1 variable is used without definition
Type 2 function is invoked without definition
Type 3 variable is redefined in the same scope
Type 4 function is redefined (in the global scope, since we don’t have nested function)
Type 5 unmatching types on both sides of assignment operator (=)
Type 6 rvalue on the left side of assignment operator
Type 7 unmatching operands, such as adding an integer to a structure variable
Type 8 the function’s return value type mismatches the declared type
Type 9 the function’s arguments mismatch the declared parameters (either types or numbers, or both)
Type 10 applying indexing operator ([...]) on non-array type variables
Type 11 applying function invocation operator (foo(...)) on non-function names Type 12 array indexing with non-integer type expression
Type 13 accessing member of non-structure variable (i.e., misuse the dot operator) Type 14 accessing an undefined structure member
Type 15 redefine the same structure type

## custom types of error
Type 16 struct is used without definition
Type 17 type is not a struct
