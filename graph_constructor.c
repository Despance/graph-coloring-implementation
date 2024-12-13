#include <stdlib.h>
#include <stdio.h>

int constructGraphFromFile(const char *filename, int ***graph, int *vertices)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        printf("Error: Could not open file %s\n", filename);
        return -1;
    }

    char line[256];
    int edges = 0;

    // Read the file line by line to determine the graph size
    while (fgets(line, sizeof(line), file))
    {
        if (line[0] == 'p')
        {
            // Problem definition line, e.g., "p edge 11 20"
            sscanf(line, "p edge %d %d", vertices, &edges);
            break; // No need to parse further for size
        }
    }

    // Allocate memory for the adjacency matrix
    *graph = (int **)malloc(*vertices * sizeof(int *));
    for (int i = 0; i < *vertices; i++)
    {
        (*graph)[i] = (int *)calloc(*vertices, sizeof(int));
    }

    // Reset file pointer to parse edges
    rewind(file);

    // Parse edges
    while (fgets(line, sizeof(line), file))
    {
        if (line[0] == 'e')
        {
            // Edge definition line, e.g., "e 1 2"
            int u, v;
            sscanf(line, "e %d %d", &u, &v);
            (*graph)[u - 1][v - 1] = 1; // Adjust for 0-based indexing
            (*graph)[v - 1][u - 1] = 1; // Since the graph is undirected
        }
    }

    fclose(file);
    return 0; // Successfully constructed the graph
}

// Function to display the adjacency matrix
void displayGraph(int **graph, int vertices)
{
    printf("Adjacency Matrix:\n");
    for (int i = 0; i < vertices; i++)
    {
        for (int j = 0; j < vertices; j++)
        {
            printf("%d ", graph[i][j]);
        }
        printf("\n");
    }
}

// Function to free dynamically allocated memory for the graph
void freeGraph(int **graph, int vertices)
{
    for (int i = 0; i < vertices; i++)
    {
        free(graph[i]);
    }
    free(graph);
}
