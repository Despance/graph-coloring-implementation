

#ifndef DSATUR_H
#define DSATUR_H
#include "graph_constructor.h"

void dsatur(int **graph, int vertices);

int *getDsaturSolution();

int *getDsaturOrderOfNodes();

double totalTime_DSatur;
#endif