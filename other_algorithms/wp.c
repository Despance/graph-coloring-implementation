#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Global variable to store the total execution time
double totalTime_wp = 0.0;

static int *wpSolution = NULL;
static int *wpOrder = NULL;

// Function to find the node with the highest saturation degree
int findHighestSaturationNode(int **graph, int *colors, bool *uncolored, int vertices)
{
    int maxSaturation = -1, selectedNode = -1;
    int *saturation = (int *)calloc(vertices, sizeof(int));
    
    for (int i = 0; i < vertices; i++)
    {
        if (!uncolored[i])
            continue;
        
        bool usedColors[vertices + 1];
        for (int j = 0; j <= vertices; j++)
            usedColors[j] = false;
        
        for (int j = 0; j < vertices; j++)
        {
            if (graph[i][j] > 0 && colors[j] != -1)
                usedColors[colors[j]] = true;
        }
        
        for (int j = 0; j <= vertices; j++)
        {
            if (usedColors[j])
                saturation[i]++;
        }
        
        if (saturation[i] > maxSaturation)
        {
            maxSaturation = saturation[i];
            selectedNode = i;
        }
    }
    free(saturation);
    return selectedNode;
}

// Welsh-Powell (WP) Graph Coloring Algorithm
int wpGraphColoring(int **graph, int vertices)
{
    clock_t start_time = clock();

    wpSolution = (int *)malloc(vertices * sizeof(int));
    wpOrder = (int *)malloc(vertices * sizeof(int));
    bool *uncolored = (bool *)malloc(vertices * sizeof(bool));
    for (int i = 0; i < vertices; i++)
    {
        wpSolution[i] = -1;
        uncolored[i] = true;
    }

    int color = 1;
    int step = 0;
    while (1)
    {
        int node = findHighestSaturationNode(graph, wpSolution, uncolored, vertices);
        if (node == -1)
            break; // No more uncolored nodes

        wpSolution[node] = color;
        uncolored[node] = false;
        wpOrder[step++] = node;

        for (int i = 0; i < vertices; i++)
        {
            if (uncolored[i])
            {
                bool conflict = false;
                for (int j = 0; j < vertices; j++)
                {
                    if (graph[i][j] > 0 && wpSolution[j] == color)
                    {
                        conflict = true;
                        break;
                    }
                }
                if (!conflict)
                {
                    wpSolution[i] = color;
                    uncolored[i] = false;
                    wpOrder[step++] = i;
                }
            }
        }
        color++;
    }

    free(uncolored);
    
    clock_t end_time = clock();
    double elapsed_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC * 1000;
    totalTime_wp += elapsed_time;
    printf("WP completed in: %.2f ms\n", elapsed_time);
    
    int maxColor = 0;
    for (int i = 0; i < vertices; i++)
    {
        if (wpSolution[i] > maxColor)
            maxColor = wpSolution[i];
    }
    
    return maxColor;
}

// Function to retrieve the WP solution
int *getWpSolution() {
    return wpSolution;
}

// Function to retrieve the order in which nodes were colored
int *getWpOrder() {
    return wpOrder;
}

// Function to print the total execution time
void printTotalTimeWP()
{
    printf("Total execution time for WP: %.2f ms\n", totalTime_wp);
}
