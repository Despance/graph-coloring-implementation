

#ifndef IMP_COLOR_H
#define IMP_COLOR_H
#include "graph_constructor.h"

void calculateImpColor(int **graph, int *nodeWeights, int nodes, int n);
int *getSolution();
int *getOrderOfNodes();

double totalTime_imp;
#endif