Colton Myers
Kyle Rich
Landon Gilbert-Bland

Red-Blue Computation
====================

This project explores the red-blue computation that we discussed in class.  It
consists of sequential, OMP, and Pthreads versions of the computation in C++, a
number of matrix files, and a Java version of the program that presents a
visualization of the computation

All tests were performed on water.eng.utah.edu.  Once logged into that server,
the files can be compiled with the following commands:

    make seq
    make omp
    make pthread

The following commands will run the resulting binaries:

    ./seq <size> <size> <filename> <converge_val> <converge_tiles>
    ./omp <size> <size> <filename> <converge_val> <converge_tiles> [num_threads]
    ./pthread <size> <size> <filename> <converge_val> <converge_tiles> [num_threads]

Note that num_threads is optional, and must divide the <size> value evenly.

An example run, using the 512x512 matrix provided, with a 16% convergence value
and 8x8 tiles, would be executed with the following commands:

    ./seq 512 512 grid512x512.txt .16 8
    ./omp 512 512 grid512x512.txt .16 8
    ./pthread 512 512 grid512x512.txt .16 8

Timing and iteration information will be automatically printed.
