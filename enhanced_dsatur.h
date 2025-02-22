
#ifndef ENHANCED_DSATUR_H
#define ENHANCED_DSATUR_H
void convertToWeightedGraph(int **graph, int vertices, int n);
int enhancedDSatur(int **graph, int *nodeWeights, int vertices);
int *enhancedDSaturSolution(int **graph, int *nodeWeights, int vertices);

#endif