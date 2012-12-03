/**
 * Colton Myers
 * Kyle Rich
 * Landon Gilbert-Bland
 *
 * Red Blue Computation with OMP parallelism
 */

#include <iostream>
#include <fstream>
#include <sys/time.h>
#include <cmath>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "omp.h"

#define BLUE 1
#define RED 2
#define WHITE 0

using namespace std;

int converges (int** grid, int rows, int cols, double conv, int tiles);

int main (int argc, char** argv) {
    int NUMTHREADS = 8;

    int rows, cols;
    double conv;
    double tiles;
    int** grid1;
    int** grid2;
    timeval start, end;
    double elapsedtime;
    int itercount = 0;

    if(argc < 6) {
        cout<<"Usage:  redblue_omp <rows> <cols> <filename> <converge_val> <converge_tiles>"<<endl;
        return 0;
    }

    rows = atoi(argv[1]);
    cols = atoi(argv[2]);
    conv = atof(argv[4]);
    tiles = atof(argv[5]);

    if(argc > 6) NUMTHREADS = atoi(argv[6]);

    cout<<"NUMTHREADS: "<<NUMTHREADS<<endl;

    grid1 = new int*[rows];
    grid2 = new int*[rows];

    for(int i = 0; i < cols; i++) {
        grid1[i] = new int[cols];
        grid2[i] = new int[cols];
    }

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

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            grid2[i][j] = WHITE;
        }
    }

    /* Actual computation */

    gettimeofday(&start, NULL);

    while(true) {
        if(converges(grid1, rows, cols, conv, tiles) != 0) break;

        int rightpos, botpos;

        // Red Iteration
        #pragma omp parallel for num_threads(NUMTHREADS)
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                if (grid1[i][j] == RED) {
                    rightpos = j+1;
                    if(rightpos >= rows) rightpos = 0;

                    if (grid1[i][rightpos] == WHITE) {
                        grid2[i][rightpos] = RED;
                    }
                    else {
                        grid2[i][j] = RED;
                    }
                }
            }
        }

        // Blue Iteration
        #pragma omp parallel for num_threads(NUMTHREADS)
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                if (grid1[i][j] == BLUE) {
                    botpos = i + 1;
                    if(botpos >= cols) botpos = 0;

                    if (grid2[botpos][j] == WHITE && grid1[botpos][j] != BLUE) {
                        grid2[botpos][j] = BLUE;
                    }
                    else {
                        grid2[i][j] = BLUE;
                    }
                }
            }
        }

        int** temp = grid1;
        grid1 = grid2;
        grid2 = temp;

        #pragma omp parallel for num_threads(NUMTHREADS)
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                grid2[i][j] = WHITE;
            }
        }

        itercount++;
    }


    gettimeofday(&end, NULL);

    /* End of computation */

    elapsedtime = (end.tv_sec - start.tv_sec) * 1000.0;
    elapsedtime += (end.tv_usec - start.tv_usec) / 1000.0;
    cout<<"Time: "<<elapsedtime<<" ms."<<endl<<endl;
    cout<<"Iterations: "<<itercount<<endl;

    for (int i = 0; i < rows; i++) {
        delete [] grid1[i];
        delete [] grid2[i];
    }
    delete [] grid1;
    delete [] grid2;
}

int converges (int** grid, int rows, int cols, double conv, int tiles) {
    int rowpart = rows / tiles;
    int colpart = cols / tiles;

    int r;
    int b;
    int tilesize = rowpart * colpart;

    for (int ii = 0; ii < tiles; ii++) {
        for (int jj = 0; jj < tiles; jj++) {
            r = 0;
            b = 0;
            for (int i = rowpart * ii; i < rowpart * (ii + 1); i++) {
                for (int j = colpart * jj; j < colpart * (jj + 1); j++) {
                    if(grid[i][j] == BLUE) b++;
                    else if(grid[i][j] == RED) r++;
                }
            }
            if(((double)r)/tilesize >= conv) return RED; //Red converge
            if(((double)b)/tilesize >= conv) return BLUE; //Blue converge
        }
    }

    // Didn't converge
    return 0;
}
