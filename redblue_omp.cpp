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
#include "omp.h"

#define NUMTHREADS 8

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


    gettimeofday(&end, NULL);

    /* End of computation */

    elapsedtime = (end.tv_sec - start.tv_sec) * 1000.0;
    elapsedtime += (end.tv_usec - start.tv_usec) / 1000.0;
    cout<<"Time: "<<elapsedtime<<" ms."<<endl<<endl;

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
