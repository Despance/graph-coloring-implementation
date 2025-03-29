#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Global variable to store the total execution time
double totalTime_rlf = 0.0;

static int *rlfSolution = NULL;
static int *rlfSolutionOrder = NULL;

// Function to find the node with the highest degree in the given subset
int findHighestDegreeNode(int **graph, bool *uncolored, int vertices)
{
    int maxDegree = -1, selectedNode = -1;
    for (int i = 0; i < vertices; i++)
    {
        if (uncolored[i])
        {
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
    }
    return selectedNode;
}

// Recursive Largest First (RLF) Algorithm
int rlfGraphColoring(int **graph, int vertices)
{
    clock_t start_time = clock();

    rlfSolution = (int *)malloc(vertices * sizeof(int));
    rlfSolutionOrder = (int *)malloc(vertices * sizeof(int));
    bool *uncolored = (bool *)malloc(vertices * sizeof(bool));
    for (int i = 0; i < vertices; i++)
    {
        rlfSolution[i] = -1;
        uncolored[i] = true;
    }

    int color = 1;
    int step = 0;
    while (1)
    {
        int node = findHighestDegreeNode(graph, uncolored, vertices);
        if (node == -1)
            break; // No more uncolored nodes

        rlfSolution[node] = color;
        rlfSolutionOrder[step++] = node;
        uncolored[node] = false;
        bool *independentSet = (bool *)malloc(vertices * sizeof(bool));
        for (int i = 0; i < vertices; i++)
            independentSet[i] = true;

        for (int i = 0; i < vertices; i++)
        {
            if (!uncolored[i])
                continue;
            if (graph[node][i] > 0)
                independentSet[i] = false;
        }

        for (int i = 0; i < vertices; i++)
        {
            if (uncolored[i] && independentSet[i])
            {
                rlfSolution[i] = color;
                rlfSolutionOrder[step++] = i;
                uncolored[i] = false;
                for (int j = 0; j < vertices; j++)
                {
                    if (graph[i][j] > 0)
                        independentSet[j] = false;
                }
            }
        }
        free(independentSet);
        color++;
    }

    free(uncolored);
    
    clock_t end_time = clock();
    double elapsed_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC * 1000;
    totalTime_rlf += elapsed_time;
    printf("RLF completed in: %.2f ms\n", elapsed_time);
    
    int maxColor = 0;
    for (int i = 0; i < vertices; i++)
    {
        if (rlfSolution[i] > maxColor)
            maxColor = rlfSolution[i];
    }
    
    return maxColor + 1;
}

// Function to return the RLF solution array
int *getRlfSolution()
{
    return rlfSolution;
}

// Function to return the RLF order of node selection
int *getRlfSolutionOrder()
{
    return rlfSolutionOrder;
}

// Function to print the total execution time
void printTotalTimeRLF()
{
    printf("Total execution time for RLF: %.2f ms\n", totalTime_rlf);
}
