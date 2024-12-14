#include <stdio.h>
#include <stdlib.h>
#include "graph_constructor.h"
#include "dsatur.h"
#include "enhanced_dsatur.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    int **graph = NULL;
    int vertices = 0;

    int result = constructGraphFromFile(filename, &graph, &vertices);
    if (result == 0)
    {
        printf("Graph constructed with %d vertices.\n", vertices);

        printf("Running DSatur algorithm...\n");

        int minColor = dsatur(graph, vertices);

        printf("DSatur algorithm completed. \n");
        printf("DSatur: %d\n", minColor);

        int *nodeWeights = (int *)calloc(vertices, sizeof(int));
        convertToWeightedGraph(graph, vertices);

        // Calculate node weights from the graph
        for (int i = 0; i < vertices; i++)
        {
            nodeWeights[i] = 0;
            for (int j = 0; j < vertices; j++)
            {
                if (graph[i][j] > 0)
                {
                    nodeWeights[i] += graph[i][j];
                }
            }
        }

        int minColorEnhanced = enhancedDSatur(graph, nodeWeights, vertices);
        printf("Enhanced DSatur algorithm completed. \n");
        printf("Enhanced DSatur: %d\n", minColorEnhanced);

        printf("minColor = %d   enhanced = %d \n", minColor, minColorEnhanced);

        free(nodeWeights);
        for (int i = 0; i < vertices; i++)
        {
            free(graph[i]);
        }
        free(graph);
    }
    else
    {
        printf("Failed to construct graph.\n");
    }

    return 0;
}