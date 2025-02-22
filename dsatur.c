#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

// Function to find the vertex with the maximum saturation degree
int findMaxSaturationVertex(int vertices, int saturation[], bool colored[], int degree[], int **graph)
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
int dsatur(int **graph, int vertices)
{
    // Time measurement start
    clock_t start_time = clock();

    int *colors = (int *)calloc(vertices, sizeof(int));     // Array to store colors for each vertex
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
        int vertex = findMaxSaturationVertex(vertices, saturation, colored, degree, graph);

        // Find the smallest available color
        bool *usedColors = (bool *)calloc(vertices + 1, sizeof(bool));
        for (int j = 0; j < vertices; j++)
        {
            if (graph[vertex][j] == 1 && colors[j] > 0)
            {
                usedColors[colors[j]] = true;
            }
        }

        int color = 1;
        while (usedColors[color])
        {
            color++;
        }

        colors[vertex] = color; // Assign the color
        colored[vertex] = true;
        printf("%d- Node %d: Color %d\n", i + 1, vertex + 1, color);

        free(usedColors); // Free the temporary array for used colors

        // Update saturation degrees of neighbors
        for (int j = 0; j < vertices; j++)
        {
            if (graph[vertex][j] == 1 && !colored[j])
            {
                bool *neighborColors = (bool *)calloc(vertices + 1, sizeof(bool));
                for (int k = 0; k < vertices; k++)
                {
                    if (graph[j][k] == 1 && colors[k] > 0)
                    {
                        neighborColors[colors[k]] = true;
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

    // Print the coloring result
    printf("DSatur Final Colors:\n");
    for (int i = 0; i < vertices; i++)
    {
        printf("Node %d: Color %d\n", i + 1, colors[i]);
    }

    // Find the maximum color used
    int maxColor = 0;
    for (int i = 0; i < vertices; i++)
    {
        if (colors[i] > maxColor)
        {
            maxColor = colors[i];
        }
    }

    free(colors);
    free(saturation);
    free(colored);
    free(degree);

    // Time measurement end
    clock_t end_time = clock();
    double time_taken = ((double)(end_time - start_time)) / CLOCKS_PER_SEC * 1000; // Milisaniye cinsinden

    printf("DSatur completed in: %.2f ms\n", time_taken);

    return maxColor + 1;
}

// This method is for verifying the coloring solution that returns the color assignment array.
int *dsaturSolution(int **graph, int vertices)
{
    clock_t start_time = clock();

    int *colors = (int *)calloc(vertices, sizeof(int));
    int *saturation = (int *)calloc(vertices, sizeof(int));
    bool *colored = (bool *)calloc(vertices, sizeof(bool));
    int *degree = (int *)calloc(vertices, sizeof(int));

    // Calculate each vertex degree
    for (int i = 0; i < vertices; i++)
    {
        for (int j = 0; j < vertices; j++)
        {
            if (graph[i][j] == 1)
                degree[i]++;
        }
    }

    // DSatur algorithm: assign colors iteratively.
    for (int i = 0; i < vertices; i++)
    {
        int vertex = findMaxSaturationVertex(vertices, saturation, colored, degree, graph);

        bool *usedColors = (bool *)calloc(vertices + 1, sizeof(bool));
        for (int j = 0; j < vertices; j++)
        {
            if (graph[vertex][j] == 1 && colors[j] > 0)
                usedColors[colors[j]] = true;
        }
        int color = 1;
        while (usedColors[color])
            color++;

        colors[vertex] = color;
        colored[vertex] = true;
        printf("%d- Node %d: Color %d\n", i + 1, vertex + 1, color);
        free(usedColors);

        // Update neighbors' saturation
        for (int j = 0; j < vertices; j++)
        {
            if (graph[vertex][j] == 1 && !colored[j])
            {
                bool *neighborColors = (bool *)calloc(vertices + 1, sizeof(bool));
                for (int k = 0; k < vertices; k++)
                {
                    if (graph[j][k] == 1 && colors[k] > 0)
                        neighborColors[colors[k]] = true;
                }
                int satCount = 0;
                for (int k = 1; k <= vertices; k++)
                {
                    if (neighborColors[k])
                        satCount++;
                }
                saturation[j] = satCount;
                free(neighborColors);
            }
        }
    }

    printf("DSatur Final Colors:\n");
    for (int i = 0; i < vertices; i++)
        printf("Node %d: Color %d\n", i + 1, colors[i]);

    // Timing print
    clock_t end_time = clock();
    double time_taken = ((double)(end_time - start_time)) / CLOCKS_PER_SEC * 1000;
    printf("DSatur completed in: %.2f ms\n", time_taken);

    free(saturation);
    free(colored);
    free(degree);
    return colors; // return the allocated array for verification
}
