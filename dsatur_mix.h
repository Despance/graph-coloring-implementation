

#ifndef DSATUR_MIX_H
#define DSATUR_MIX_H
#include "graph_constructor.h"

void dsatur_mix(int **graph, int vertices, int k);

int *getMixSolution();

int *getMixOrderOfNodes();

extern double totalTime_mix;
#endif