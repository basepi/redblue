/**
 * Colton Myers
 * Kyle Rich
 * Landon Gilbert-Bland
 *
 * Red Blue Computation with pthreads(<3) parallelism
 */

#include <iostream>
#include <fstream>
#include <sys/time.h>
#include <cmath>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define WHITE 0
#define BLUE 1
#define RED 2

using namespace std;

int iterations = 0;

typedef struct {
    int startx;           // start x position of a tile
    int endx;             // end x position of a tile
    int starty;           // start y position of a tile
    int endy;             // end y position of a tile
    double converge_percent; // convergence percent
    int tile_size;     // total size of a tile
    int rows;             // number of rows
    int** grid;           // original grid, shared between threads
    int** newgrid;        // copy grid, shared between threads
    pthread_barrier_t *barrier; // barrier to synchronize threads
    bool *finished;        // boolean used to indicate to all threads when 1 converges
} tile_info;


void* run(void* tile) {
    tile_info *t = (tile_info*)tile;
    double converge_percent = t->converge_percent;
    int tile_size = t->tile_size;
    int rows = t->rows;
    int startx = t->startx;
    int endx = t->endx;
    int starty = t->starty;
    int endy = t->endy;
    int** grid = t->grid;
    int** newgrid = t->newgrid;
    pthread_barrier_t *barrier = t->barrier;
    bool *finished = t->finished;

    // Initially check for convergence
    int bluecount = 0;
    int redcount = 0;
    for(int i=startx; i<endx; i++) {
        for(int j=starty; j<endy; j++) {
            if(grid[i][j] == BLUE)
                bluecount++;
            else if(grid[i][j] == RED)
                redcount++;
        }
    }
    if(((double)redcount / tile_size) >= converge_percent)
        *finished = true;
    else if(((double)bluecount / tile_size) >= converge_percent)
        *finished = true;

    // Barrier to insure that if any tile converges, all threads stop
    pthread_barrier_wait(barrier);

    // Run until one section converges
    while(!*finished) {
        printf("%d\n", iterations);
        // Iterate red 1 step
        for(int i=startx; i<endx; i++) {
            for(int j=starty; j<endy; j++) {
                if(grid[i][j] == RED)  {
                    // deal with wrapping
                    int rightpos = j+1;
                    if(rightpos >= rows)
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
        for(int i=startx; i<endx; i++) {
            for(int j=starty; j<endy; j++) {
                if(grid[i][j] == BLUE)  {
                    // deal with wrapping
                    int botpos = i+1;
                    if(botpos >= rows)
                        botpos = 0;

                    if(newgrid[botpos][j] == WHITE && grid[botpos][j] != BLUE)
                        newgrid[botpos][j] = BLUE;
                    else
                        newgrid[i][j] = BLUE;
                }
            }
        }

        // Barrier
        pthread_barrier_wait(barrier);

        // Set grid to newgrid, and reset this tile of newgrid to white.
        // To save time on memcopies, just swap the pointers for grid and
        // newgrid. Because each thread will do this in the same section
        // (guarenteed by the barriers) this shouldn't cause any problems
        int** tmp = grid;
        grid = newgrid;
        newgrid = tmp;
        for(int i=startx; i<endx; i++) {
            for(int j=starty; j<endy; j++) {
                newgrid[i][j] = WHITE;
            }
        }

        // Check for convergence (if this tile contains more then x% one
        // color or another)
        double bluecount = 0;
        double redcount = 0;
        for(int i=startx; i<endx; i++) {
            for(int j=starty; j<endy; j++) {
                if(grid[i][j] == BLUE)
                    bluecount++;
                else if(grid[i][j] == RED)
                    redcount++;
            }
        }
        if(((double)redcount / tile_size) >= converge_percent)
            *finished = true;
        else if(((double)bluecount / tile_size) >= converge_percent)
            *finished = true;

        // Barrier
        if(pthread_barrier_wait(barrier) != 0)
            iterations++;
    }
}

int main (int argc, char** argv) {
    // stuff
    int rows, cols;
    double conv;
    int tiles;
    int** grid1;
    int** grid2;
    timeval start, end;
    double elapsedtime;


    // Command line arguements
    if(argc < 5) {
        cout<<"Usage:  redblue_omp <rows> <cols> <filename> <converge_val> <converge_tiles>"<<endl;
        return 0;
    }
    rows = atoi(argv[1]);
    cols = atoi(argv[2]);
    conv = atof(argv[4]);
    tiles = atof(argv[5]);

    // Space for grids
    grid1 = new int*[rows];
    grid2 = new int*[rows];
    for(int i = 0; i < cols; i++) {
        grid1[i] = new int[cols];
        grid2[i] = new int[cols];
    }

    // Init grid based on file
    ifstream gridfile(argv[3]);
    if(!(gridfile.is_open())) {
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

    bool finished = false;
    pthread_barrier_t barrier;
    if(pthread_barrier_init(&barrier, NULL, tiles*2) != 0)
        printf("error creating barrier\n");
    pthread_t threads[tiles*2];
    tile_info t[tiles*2];

    // Create sturcts for each thread
    int count = 0;
    for(int i=0; i<rows; i+=tiles) {
        for(int j=0; j<rows; j+=tiles) {
            // Tile positions
            t[count].startx = i;
            t[count].endx = i + tiles;
            t[count].starty = j;
            t[count].endy = j + tiles;

            // All other info needed for running thread
            t[count].converge_percent = conv;
            t[count].tile_size = tiles*tiles;
            t[count].rows = rows;
            t[count].grid = grid1;
            t[count].newgrid = grid2;
            t[count].barrier = &barrier;
            t[count].finished = &finished;

            // Update position counter. Probably a better math way to do this but meh.
            count++;
        }
    }

    for(int i=0; i<tiles*2; i++) {
        pthread_create(&threads[i], NULL, &run, (void*) &t[i]);
    }

    for(int i=0; i<tiles*2; i++) {
        pthread_join(threads[i], NULL);
    }

    gettimeofday(&end, NULL);

    /* End of computation */

    elapsedtime = (end.tv_sec - start.tv_sec) * 1000.0;
    elapsedtime += (end.tv_usec - start.tv_usec) / 1000.0;
    cout<<"Time: "<<elapsedtime<<" ms."<<endl<<endl;
    cout<<"Iterations: "<<iterations<<endl;

}
