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
    *vertices = 0; // Başlangıç değeri atandı

    // Read the file line by line to determine the graph size
    while (fgets(line, sizeof(line), file))
    {
        if (line[0] == 'p')
        {
            if (sscanf(line, "p edges %d %d", vertices, &edges) != 2 || *vertices <= 0)
            {
                printf("Error: Invalid problem definition line in %s\n", filename);
                fclose(file);
                return -1;
            }
            break;
        }
    }

    if (*vertices == 0)
    {
        printf("Error: No valid 'p edge' line found in %s\n", filename);
        fclose(file);
        return -1;
    }

    // Allocate memory for the adjacency matrix
    *graph = (int **)malloc(*vertices * sizeof(int *));
    if (*graph == NULL)
    {
        printf("Error: Memory allocation failed for graph.\n");
        fclose(file);
        return -1;
    }

    for (int i = 0; i < *vertices; i++)
    {
        (*graph)[i] = (int *)calloc(*vertices, sizeof(int));
        if ((*graph)[i] == NULL)
        {
            printf("Error: Memory allocation failed for row %d\n", i);
            fclose(file);
            return -1;
        }
    }

    // Reset file pointer to parse edges
    rewind(file);

    // Parse edges
    while (fgets(line, sizeof(line), file))
    {
        if (line[0] == 'e')
        {
            int u, v;
            if (sscanf(line, "e %d %d", &u, &v) != 2 || u <= 0 || v <= 0 || u > *vertices || v > *vertices)
            {
                printf("Error: Invalid edge definition in %s: %s\n", filename, line);
                continue;
            }
            (*graph)[u - 1][v - 1] = 1; // Adjust for 0-based indexing
            (*graph)[v - 1][u - 1] = 1; // Since the graph is undirected
        }
    }

    fclose(file);
    return 0; // Successfully constructed the graph
}
