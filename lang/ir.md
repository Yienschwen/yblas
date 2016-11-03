# IR Reference

Currently all operations are based AVX instruction set if needed.

## Matrix building and methods

### litmat

Literal matrix construction, like `[1 2; 3 4]`.
The result will be pushed onto the operand stack.

format: `litmat HEIGHT WIDTH entries...`  
example: 

    litmat 2 3 1 2 3 4 5 6
 
Which is equivalent to `[1 2 3; 4 5 6]`.

### set

Pop the top operand on the operand stack and assign it
to the variable of given identifier.

If the operand is a reference of a matrix, the copy of the matrix
will be taken and assigned.

format: `set IDENTIFIER`  
example: 

    litmat 2 3 1 2 3 4 5 6
    set A
   
And `A` will be equal to `[1 2 3; 4 5 6]`.

### part

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

### pushref

Take the reference of the matrix of given identifier and push it onto the stack.

format: `pushref IDENTIFIER`

example:

    litmat 2 2 1 2 3 4
    set A
    pushref A
    set B
    litmat A 3 3 1 2 3 4 5 6 7 8 9
    set A

`A` will be `[1 2 3; 4 5 6; 7 8 9]` at the end, 
while `B` stays being `[1 2 3 4]`.

### mfset1avx

Build a matrix of given width and height and all entries equal to the given float.

**Based on AVX instruction set.**

format: `mfset1avx HEIGHT WIDTH VALUE`

example: 

    mfset1avx 2 3 2
    set A

`A` will be `[2 2 2; 2 2 2]`.

## Matrix arithmetics

### mfaddavx

Pops two operands on the stack and take the sum of them, and push the sum back to
the stack.

**Based on AVX instruction set.**

format: `mfaddavx`

example:

    litmat 2 2 1 2 3 4
    litmat 2 2 2 4 6 8
    mfaddavx
    set A

`A` will be `[3 6; 9 12]`.

### mfsubavx

Pop one operand on stack as minuend, and another as subtrahend, then subtraction
is carried out and the difference is pushed back to the stack.

**Based on AVX instruction set.**

format: `mfsubavx`

example:

    litmat 2 2 1 2 3 4
    litmat 2 2 2 4 6 8
    mfsubavx
    set A

`A` will be `[1 2; 3 4]`.

### mfdmulavx

Pops two operands on the stack and take the "dot product" of them,
which means the each entry of the "product" is the product of corresponding
entries of the same coordinate in two operand matrices, as the one in the product.

And finally the result will be pushed back to the stack.

**Based on AVX instruction set.**

format: `mfdmulavx`

example:

    litmat 2 2 1 2 3 4
    litmat 2 2 2 4 6 8
    mfdmulavx
    set A

`A` will be `[2 8; 18 32]`.

### mfddivavx

Pop one operand on stack as dividend, and another as divisor,
and the take the "dot quotient" of them, 
which means the each entry of the "quotient" is the quotient of corresponding
entries of the same coordinate in two operand matrices, as the one in the product.

And finally the result will be pushed back to the stack.

**Based on AVX instruction set.**

format: `mfddivavx`

example:

    litmat 2 2 1 2 3 4
    litmat 2 2 2 4 6 8
    mfddivavx
    set A

`A` will be `[2 2; 2 2]`.

## Helper instructions

###  print

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

### clearvars

Remove the matrices of given identifiers from memory.

format: `clearvars N IDENTIFIER ...`  
example:

    litmat	3 3 1 2 3 4 5 6 7 8 9
    set A
    part A 1 2 1 2
    set B
    clearvars 2 A B

The `clearvars` at the end will remove both `A` and `B`.

### clear

Remove all matrices from memory.

format: `clear`  
example:

    litmat	3 3 1 2 3 4 5 6 7 8 9
    set A
    part A 1 2 1 2
    set B
    clear

The `clear` at the end will remove both `A` and `B`.