#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "graph_constructor.h"
#include "enhanced_dsatur.h"

// Function to save the weighted graph to a file
void saveWeightedGraph(const char *filename, int **graph, int vertices)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        printf("Error: Could not open file %s\n", filename);
        return;
    }

    // Write edges
    for (int i = 0; i < vertices; i++)
    {
        for (int j = i + 1; j < vertices; j++) // Avoid duplicate edges
        {
            if (graph[i][j] > 0)
            {
                fprintf(file, "e\t%d\t%d\n", i + 1, j + 1);
            }
        }
    }

    // Write node weights
    for (int i = 0; i < vertices; i++)
    {
        int nodeWeight = 0;
        for (int j = 0; j < vertices; j++)
        {
            if (graph[i][j] > 0)
            {
                nodeWeight += graph[i][j];
            }
        }
        fprintf(file, "n\t%d\t%d\n", i + 1, nodeWeight);
    }

    fclose(file);
}
