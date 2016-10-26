# yblas

## Descrpition

*   This is a Matlab-like toy of doing linear algebra related 
    calculations, containing webpage-based server and core 
    computation library. 
*   Mostly it will adopt Matlab grammar.
*   SSE (or AVX if available) and OpenMP will be used, just as a trial
    optimization using SIMD and parallelization.
*   No GUI version of client will be developed, which is too time-consuming
    for me. Single-user local server would play that role.

## Structure

This is just the current working plan of this project, may be changed in following commits. 

    yblas  
    +- yblas_core
    |   +- [basic 2D matrix] <= Working on it 
    |   +- blas
    |   +- toolbox
    |       +- Computer Vision toolbox
    |       +- Signals and Systems toolbox
    |       +- ......
    |
    +- yblas_server
        +- util
        |   +- parser
        |   +- LLVM-based compiler
        |
        +- single-user local server
        +- multi-user server (+ authentication)
        +- ......

## License
GPLv3
