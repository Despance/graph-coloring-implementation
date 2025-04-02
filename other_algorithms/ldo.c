#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Global variable to store the total execution time
double totalTime_ldo = 0.0;

static int *ldoSolution = NULL;
static int *ldoOrder = NULL;

// Function to find the node with the highest degree order
int findHighestDegreeNodeLDO(int **graph, bool *uncolored, int vertices)
{
    int maxDegree = -1, selectedNode = -1;
    for (int i = 0; i < vertices; i++)
    {
        if (!uncolored[i])
            continue;
        
        int degree = 0;
        for (int j = 0; j < vertices; j++)
        {
            if (graph[i][j] > 0 && uncolored[j])
                degree++;
        }
        
        if (degree > maxDegree)
        {
            maxDegree = degree;
            selectedNode = i;
        }
    }
    return selectedNode;
}

// Largest Degree Ordering (LDO) Graph Coloring Algorithm
int ldoGraphColoring(int **graph, int vertices)
{
    clock_t start_time = clock();

    ldoSolution = (int *)malloc(vertices * sizeof(int));
    ldoOrder = (int *)malloc(vertices * sizeof(int));
    bool *uncolored = (bool *)malloc(vertices * sizeof(bool));
    for (int i = 0; i < vertices; i++)
    {
        ldoSolution[i] = -1;
        uncolored[i] = true;
    }

    int color = 1; 
    int step = 0;
    while (1)
    {
        int node = findHighestDegreeNodeLDO(graph, uncolored, vertices);
        if (node == -1)
            break; // No more uncolored nodes

        ldoSolution[node] = color;
        uncolored[node] = false;
        ldoOrder[step++] = node;

        for (int i = 0; i < vertices; i++)
        {
            if (uncolored[i])
            {
                bool conflict = false;
                for (int j = 0; j < vertices; j++)
                {
                    if (graph[i][j] > 0 && ldoSolution[j] == color)
                    {
                        conflict = true;
                        break;
                    }
                }
                if (!conflict)
                {
                    ldoSolution[i] = color;
                    uncolored[i] = false;
                    ldoOrder[step++] = i;
                }
            }
        }
        color++;
    }


    free(uncolored);
    
    clock_t end_time = clock();
    double elapsed_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC * 1000;
    totalTime_ldo += elapsed_time;
    printf("LDO completed in: %.2f ms\n", elapsed_time);
    
    int maxColor = 0;
    for (int i = 0; i < vertices; i++)
    {
        if (ldoSolution[i] > maxColor)
            maxColor = ldoSolution[i];
    }
    
    return maxColor;
}

// Function to retrieve the LDO solution
int *getLdoSolution() {
    return ldoSolution;
}

// Function to retrieve the order in which nodes were colored
int *getLdoOrder() {
    return ldoOrder;
}

// Function to print the total execution time
void printTotalTimeLDO()
{
    printf("Total execution time for LDO: %.2f ms\n", totalTime_ldo);
}
