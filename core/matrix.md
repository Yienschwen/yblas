# yblas::core::matrix

## Matlab matrix grammar

* literal building
    
    a = [1 2 3; 4 5 6; 7 8 9];

* element picking

    a(3, 1) // = 7  

* reference  

    b = a(1:2, 1:2);
    a(1:2,1:2) = [-1 -2; -4 -5];
    % a = [-1 -2 3; -4 -5 6; 7 8 9];
    % b = [1 2; 4 5];

* row / column deleting

    a = [1 2 3; 4 5 6; 7 8 9];
    a(2,:) = [];
    // a = [1 2 3; 7 8 9];
    a(:,2) = [];
    // a = [1 3; 7 9];

## Need to implement in C++

* reference and dereference
* slicing
* insert
* more than 2D: arithmetic??
* arithmetic