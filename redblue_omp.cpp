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

int main (int argc, char** argv) {

    int rows, cols;
    int** grid1;
    int** grid2;

    if(argc < 4) {
        cout<<"Usage:  redblue_omp <rows> <cols> <filename>"<<endl;
        return 0;
    }

    rows = atoi(argv[1]);
    cols = atoi(argv[2]);

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
}
