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

typedef struct {
    int converge_precent; // convergence percent
    double tile_size;     // total size of a tile
    int min;              // start position of a tile
    int max;              // end position of a tile
    int** grid;           // original grid, shared between threads
    int** newgrid;        // copy grid, shared between threads
    pthread_barrier_t *barrier; // barrier to synchronize threads
    bool finished;        // boolean used to indicate to all threads when 1 converges
} tile_info;

void* run(void* tile) {
    tile_info *t = (tile_info*)tile;

    // Initially check for convergence
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

    // Barrier to insure that if any tile converges, all threads stop
    pthread_barrier_wait(barrier);

    // Run until one section converges
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
                        newgrid[i][rightpos] = RED;
                    else
                        newgrid[i][j] = RED;
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

                    if(newgrid[botpos][j] == WHITE && grid[botpos][j] != BLUE)
                        newgrid[botpos][j] = BLUE;
                    else
                        newgrid[i][j] = BLUE;
                }
            }
        }

        // Barrier
        pthread_barrier_wait(barrier)

        // Set grid to newgrid, and reset this tile of newgrid to white.
        // To save time on memcopies, just swap the pointers for grid and
        // newgrid. Because each thread will do this in the same section
        // (guarenteed by the barriers) this shouldn't cause any problems
        int** tmp = grid;
        grid = newgrid;
        newgrid = tmp;
        for(int i=min; i<max; i++) {
            for(int j=min; j<max; j++) {
                newgrid[i][j] = WHITE;
            }
        }

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

        // Barrier
        pthread_barrier_wait(barrier);
    }
}
