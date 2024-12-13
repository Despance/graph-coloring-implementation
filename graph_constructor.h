#ifndef GRAPH_CONSTRUCTOR_H
#define GRAPH_CONSTRUCTOR_H

#define MAX_VERTICES 100 // Set a maximum number of vertices for flexibility
int constructGraphFromFile(const char *filename, int ***graph, int *vertices);
void displayGraph(int **graph, int vertices);
void freeGraph(int **graph, int vertices);

#endif