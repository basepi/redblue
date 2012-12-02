/**
 * Colton Myers
 * Kyle Rich
 * Landon Gilbert-Bland
 *
 * Red Blue Computation with OMP parallelism
 */

#include <iostream>
#include <cmath>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include "pthread.h"

#define NUMTHREADS 8
#define WHITE 0
#define BLUE 1
#define RED 2

using namespace std;

int converges (int** grid, int rows, int cols, double conv, int tiles);

int main (int argc, char** argv) {

    int rows, cols;
    double conv;
    double tiles;
    int** grid1;
    int** grid2;
    timeval start, end;
    double elapsedtime;

    if(argc < 5) {
        cout<<"Usage:  redblue_omp <rows> <cols> <filename> <converge_val> <converge_tiles>"<<endl;
        return 0;
    }

    rows = atoi(argv[1]);
    cols = atoi(argv[2]);
    conv = atof(argv[4]);
    tiles = atof(argv[5]);

    grid1 = new int[rows][cols];
    grid2 = new int[rows][cols];

    ifstream gridfile(argv[3]);
    if(!(grifile.is_open())) {
        cout<<"Error:  file not found"<<endl;
        return 0;
    }

    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            gridfile >> grid1[i][j];
        }
    }

    gridfile.close();


    /* Actual computation */

    gettimeofday(&start, NULL);

    for(i=0; i<numOfTiles; i++) {
            pthread_create(&threads[i], NULL, &run, (void*) &s[i]);
    }

    gettimeofday(&end, NULL);

    /* End of computation */

    elapsedtime = (end.tv_sec - start.tv_sec) * 1000.0;
    elapsedtime += (end.tv_usec - start.tv_usec) / 1000.0;
    cout<<"Time: "<<elapsedtime<<" ms."<<endl<<endl;
}

// Convergence
// initially bread up threads
void* run(void* t) {
    new_grid = WHITE;
    int min;
    int max;
    int converge_percent;
    pthread_barrier_t *barrier;

    while(!finished) {

        // Iterate red 1 step
        for(int i=min; i<max; i++) {
            for(int j=min; j<max; j++) {
                if(grid[i][j] == RED)  {
                    // deal with wrapping
                    int rightpos = j+1;
                    if(rightpos >= size)
                        rightpos = 0;

                    if(grid[i][rightpos] == WHITE)
                        new_grid[i][rightpos] = RED;
                    else
                        new_grid[i][j] = RED;
                }
            }
        }

        // Barrier
        pthread_barrier_wait(barrier);

        // Iterate blue 1 step
        for(int i=min; i<max; i++) {
            for(int j=min; j<max; j++) {
                if(grid[i][j] == BLUE)  {
                    // deal with wrapping
                    int botpos = i+1;
                    if(botpos >= size)
                        botpos = 0;

                    if(new_grid[botpos][j] == WHITE && grid[botpos][j] != BLUE)
                        new_grid[botpos][j] = BLUE;
                    else
                        new_grid[i][j] = BLUE;
                }
            }
        }

        // Barrier
        pthread_barrier_wait(barrier)

        // Check for convergence (if this tile contains more then x% one
        // color or another)
        double bluecount = 0;
        double redcount = 0;
        for(int i=min; i<max; i++) {
            for(int j=min; j<max; j++) {
                if(grid[i][j] == BLUE)
                    bluecount++;
                else if(grid[i][j] == RED)
                    redcount++;
            }
        }
        if((redcount / total) >= converge_percent)
            finished = true;
        else if((bluecount / total) >= converge_percent)
            finished = true;

        // swap out pointers for grids, and whiteout your section of the newgrid
        grid = newgrid;
        newgrid = WHITE;

        // Barrier
        pthread_barrier_wait(barrier);
    }
}



void* run(void* t) {
    copy_grid; // copy of the original grid that we can modify
    new_grid = WHITE;
    int max; // end of your tile
    int min; //start of your tile
    int size; //size of the grid

    // Iterate red 1 step
    // First, deal with the borders by checking checking over our border to other cells
    for(int j=min; j<max; j++) {
        if(grid[max][j] == RED) {
            // Wrap if on the end of the grid
            int rightpos = j+1;
            if(rightpos >= size)
                rightpos = 0;

            // If is already white, then we can move there
            if(grid[max][rightpos] == WHITE) {
                new_grid[max][rightpos] = RED;
            }

            // If red, then we need to go farther to check. Traverse the border
            // till you can see for sure if it will be able to move or not
            else if(grid[max][rightpos] == RED) {

                int checkpos = rightpos + 1; // The next position in the grid to check
                while(true) {
                    if(grid[max][checkpos] == WHITE) {
                        new_grid[max][rightpos] = RED;
                        break;
                    }
                    else if(grid[max][checkpos] == RED) {
                        checkpos++;
                        if(checkpos >= size)
                            checkpos = 0;
                        if(checkpos == j) { // Row is all red
                            new_grid[max][rightpos] = RED;
                            break;
                        }
                    }
                    else // else is blue, so do nothing
                        break;
                }
            }
            // else is blue, so do nothing
        }
    }

    // Now, deal with everything in your array that isn't a border
    for(int i=min; i<max; i++) {
        for(int j=max-1; j>=min; j--) {
            if(grid[i][j] == RED)  {
                // deal with wrapping
                int rightpos = j+1;
                if(rightpos >= size)
                    rightpos = 0;

                if(grid[i][rightpos] == WHITE) {
                    new_grid[i][rightpos] = RED;
                }
            }
        }
    }

    // Block till all reds are done
    pthread_barrier_TODO()

    // Iterate blue down 1 step
    // Check convergence
    // Block
}

int converges (int** grid, int rows, int cols, double conv, int tiles) {
    int rowpart = rows / tiles;
    int colpart = cols / tiles;

    int r;
    int b;
    int tilesize = rowpart * colpart

    for (int ii = 0; ii < tiles; ii++) {
        for (int jj = 0; jj < tiles; jj++) {
            r = 0;
            b = 0;
            w = 0;
            for (int i = rowpart * ii; i < rowpart * (ii + 1); i++) {
                for (int j = colpart * jj; j < colpart * (jj + 1); j++) {
                    if(grid[i][j] == 1) b++;
                    else if(grid[i][j] == 2) r++;
                }
            }
            if(((double)r)/tilesize >= conv) return 2; //Red converge
            if(((double)b)/tilesize >= conv) return 1; //Blue converge
        }
    }

    // Didn't converge
    return 0;
}
