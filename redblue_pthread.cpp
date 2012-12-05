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
#include <pthread.h>

#define BLUE 1
#define RED 2
#define WHITE 0

using namespace std;

int** grid1;
int** grid2;

int converges(int** grid, int rows, int cols, double conv, int tiles);
int threadBoundary(int threads);
void* iterateGrid(void* arg);

struct thread_info {
    pthread_t thread_id;
    int color;
    int il;
    int iu;
    int jl;
    int ju;
    int bound;
};

int main(int argc, char** argv) {
    int NUMTHREADS = 8;
    int rows, cols;
    double conv;
    int tiles;

    timeval start, end;
    double elapsedtime;
    int itercount = 0;

    if (argc < 6) {
        cout << "Usage:  redblue_pthread <rows> <cols> <filename> <converge_val> <converge_tiles> [<num_threads>]" << endl;
        return 0;
    }

    rows = atoi(argv[1]);
    cols = atoi(argv[2]);
    conv = atof(argv[4]);
    tiles = atoi(argv[5]);

    if (argc > 6) NUMTHREADS = atoi(argv[6]);


    cout << "NUMTHREADS: " << NUMTHREADS << endl;

    grid1 = new int*[rows];
    grid2 = new int*[rows];

    for (int i = 0; i < cols; i++) {
        grid1[i] = new int[cols];
        grid2[i] = new int[cols];
    }

    struct thread_info *tinfo;
    tinfo = (thread_info*) calloc(NUMTHREADS, sizeof (struct thread_info));

    ifstream gridfile(argv[3]);
    if (!(gridfile.is_open())) {
        cout << "Error:  file not found" << endl;
        return 0;
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            gridfile >> grid1[i][j];
        }
    }

    gridfile.close();
    cout << "File loaded" << endl;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            grid2[i][j] = WHITE;
        }
    }

    int jDim = threadBoundary(NUMTHREADS);
    int iDim = NUMTHREADS / jDim;
    cout << "iDim: " << iDim << ", jDim: " << jDim << endl;

    /* Actual computation */

    gettimeofday(&start, NULL);

    int tNum;
    for (int i = 0; i < iDim; i++) {
        for (int j = 0; j < jDim; j++) {
            tNum = i * jDim + j;
            tinfo[tNum].il = (int) ((float) rows / (float) iDim * i);
            tinfo[tNum].iu = (int) ((float) rows / (float) iDim * ((float) i + 1));
            tinfo[tNum].jl = (int) ((float) rows / (float) jDim * (float) j);
            tinfo[tNum].ju = (int) ((float) rows / (float) jDim * ((float) j + 1));
        }
    }

    while (!converges(grid1, rows, cols, conv, tiles)) {
        for (int i = 0; i < iDim; i++) {
            for (int j = 0; j < jDim; j++) {
                tNum = i * jDim + j;
                tinfo[tNum].color = RED;
                tinfo[tNum].bound = rows;
                pthread_create(&tinfo[tNum].thread_id, NULL, iterateGrid, &tinfo[tNum]);
            }
        }

        for (int i = 0; i < NUMTHREADS; i++) {
            pthread_join(tinfo[i].thread_id, NULL);
        }

        for (int i = 0; i < iDim; i++) {
            for (int j = 0; j < jDim; j++) {
                tNum = i * jDim + j;
                tinfo[tNum].color = BLUE;
                tinfo[tNum].bound = cols;
                pthread_create(&tinfo[tNum].thread_id, NULL, iterateGrid, &tinfo[tNum]);
            }
        }

        for (int i = 0; i < NUMTHREADS; i++) {
            pthread_join(tinfo[i].thread_id, NULL);
        }

        int** temp = grid1;
        grid1 = grid2;
        grid2 = temp;

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
    cout << "Time: " << elapsedtime << " ms." << endl << endl;
    cout << "Iterations: " << itercount << endl;

    for (int i = 0; i < rows; i++) {
        delete [] grid1[i];
        delete [] grid2[i];
    }
    delete [] grid1;
    delete [] grid2;
}

void* iterateGrid(void* arg) {
    struct thread_info *tinfo = (struct thread_info *) arg;
    int position = 0;
    for (int i = tinfo->il; i < tinfo->iu; i++) {
        for (int j = tinfo->jl; j < tinfo->ju; j++) {
            if (tinfo->color == RED) {
                if (grid1[i][j] == RED) {
                    position = j + 1;
                    if (position >= tinfo->bound)
                        position = 0;
                    if (grid1[i][position] == WHITE) {
                        grid2[i][position] = RED;
                    } else {
                        grid2[i][j] = RED;
                    }
                }
            } else if (tinfo->color == BLUE) {
                if (grid1[i][j] == BLUE) {
                    position = i + 1;
                    if (position >= tinfo->bound)
                        position = 0;
                    if (grid2[position][j] == WHITE && grid1[position][j] != BLUE) {
                        grid2[position][j] = BLUE;
                    } else {
                        grid2[i][j] = BLUE;
                    }
                }
            }
        }
    }
    return 0;
}

int converges(int** grid, int rows, int cols, double conv, int tiles) {
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
                    if (grid[i][j] == BLUE) b++;
                    else if (grid[i][j] == RED) r++;
                }
            }
            if (((double) r) / tilesize >= conv) return RED; //Red converge
            if (((double) b) / tilesize >= conv) return BLUE; //Blue converge
        }
    }

    // Didn't converge
    return 0;
}

int threadBoundary(int threads) {
    int result = (int) sqrt(threads);
    while (result) {
        if (threads % result == 0)
            return result;
        result--;
    }
    return 0;
}
