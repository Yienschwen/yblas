# IR Reference

## litmat

Literal matrix construction, like `[1 2; 3 4]`.
The result will be pushed onto the operand stack.

format: `litmat HEIGHT WIDTH entries...`  
example: 

    litmat 2 3 1 2 3 4 5 6
 
Which is equivalent to `[1 2 3; 4 5 6]`.

## set

Pop the top operand on the operand stack and assign it
to the variable of given identifier.

format: `set IDENTIFIER`  
example: 

    litmat 2 3 1 2 3 4 5 6
    set A
   
And `A` will be equal to `[1 2 3; 4 5 6]`.

## part

Take the copy of the given matrix within the given boundaries,
and push it onto the operand stack.

format: `part IDENTIFIER ROW_START ROW_END COL_START COL_END`  
example:

    litmat 3 4 1 2 3 4 5 6 7 8 9 10 11 12
    set A
    part A 1 3 2 3
    set B

`A` equals `[1 2 3 4; 5 6 7 8; 9 10 11 12]`, and `B` takes part
of `A`, as `B = A(1:3, 2:3)`, thus equals `[2 3; 6 7; 10 11]`.

##  print

Print the value of the given matrix identifier.

format: `print IDENTIFIER`  
example:

    litmat 2 2 1 2 3 4
    set A
    print A

This will generate the following output:

    A =
        1       2
        3       4

## clearvars

Remove the matrices of given identifiers from memory.

format: `clearvars N IDENTIFIER ...`  
example:

    litmat	3 3 1 2 3 4 5 6 7 8 9
    set A
    part A 1 2 1 2
    set B
    clearvars 2 A B

The `clearvars` at the end will remove both `A` and `B`.

## clear

Remove all matrices from memory.

format: `clear`  
example:

    litmat	3 3 1 2 3 4 5 6 7 8 9
    set A
    part A 1 2 1 2
    set B
    clear

The `clear` at the end will remove both `A` and `B`.