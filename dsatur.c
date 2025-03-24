#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

int *dsaturSolution;
int *dSaturOrder;

time_t exec_time;
double totalTime_DSatur = 0.0;
int *getDsaturSolution()
{
    return dsaturSolution;
}

int *getDsaturOrderOfNodes()
{
    return dSaturOrder;
}

// Function to find the vertex with the maximum saturation degree
int findMaxSaturationVertex(int vertices, int saturation[], bool colored[], int degree[])
{
    int maxSaturation = -1;
    int maxDegree = -1;
    int vertex = -1;

    for (int i = 0; i < vertices; i++)
    {
        if (!colored[i])
        {
            if (saturation[i] > maxSaturation ||
                (saturation[i] == maxSaturation && degree[i] > maxDegree))
            {
                maxSaturation = saturation[i];
                maxDegree = degree[i];
                vertex = i;
            }
        }
    }
    return vertex;
}

// DSatur algorithm implementation
void dsatur(int **graph, int vertices)
{

    exec_time = clock();

    dsaturSolution = (int *)calloc(vertices, sizeof(int));  // Array to store colors for each vertex
    dSaturOrder = (int *)calloc(vertices, sizeof(int));     // Array to store the order of nodes
    int *saturation = (int *)calloc(vertices, sizeof(int)); // Saturation degree for each vertex
    bool *colored = (bool *)calloc(vertices, sizeof(bool)); // Whether a vertex is colored
    int *degree = (int *)calloc(vertices, sizeof(int));     // Degree of each vertex

    // Calculate the degree of each vertex
    for (int i = 0; i < vertices; i++)
    {
        for (int j = 0; j < vertices; j++)
        {
            if (graph[i][j] == 1)
            {
                degree[i]++;
            }
        }
    }

    // Start coloring the graph
    for (int i = 0; i < vertices; i++)
    {
        int vertex = findMaxSaturationVertex(vertices, saturation, colored, degree);

        // Find the smallest available color
        bool *usedColors = (bool *)calloc(vertices + 1, sizeof(bool));
        for (int j = 0; j < vertices; j++)
        {
            if (graph[vertex][j] == 1 && dsaturSolution[j] > 0)
            {
                usedColors[dsaturSolution[j]] = true;
            }
        }

        int color = 1;
        while (usedColors[color])
        {
            color++;
        }

        dsaturSolution[vertex] = color; // Assign the color
        dSaturOrder[i] = vertex;        // Store the order of nodes
        colored[vertex] = true;

        free(usedColors); // Free the temporary array for used colors

        // Update saturation degrees of neighbors
        for (int j = 0; j < vertices; j++)
        {
            if (graph[vertex][j] == 1 && !colored[j])
            {
                bool *neighborColors = (bool *)calloc(vertices + 1, sizeof(bool));
                for (int k = 0; k < vertices; k++)
                {
                    if (graph[j][k] == 1 && dsaturSolution[k] > 0)
                    {
                        neighborColors[dsaturSolution[k]] = true;
                    }
                }

                int saturationCount = 0;
                for (int k = 1; k <= vertices; k++)
                {
                    if (neighborColors[k])
                    {
                        saturationCount++;
                    }
                }

                saturation[j] = saturationCount;

                free(neighborColors); // Free the temporary array for neighbor colors
            }
        }
    }

    totalTime_DSatur = (double)(clock() - exec_time) / CLOCKS_PER_SEC;

    free(saturation);
    free(colored);
    free(degree);
}
