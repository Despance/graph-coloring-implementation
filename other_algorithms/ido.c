#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Global variable to store the total execution time
double totalTime_ido = 0.0;

static int *idoSolution = NULL;
static int *idoOrder = NULL;

// Function to find the node with the highest incidence degree
int findHighestIncidenceDegreeNode(int **graph, bool *uncolored, int vertices)
{
    int maxIncidence = -1, selectedNode = -1;
    for (int i = 0; i < vertices; i++)
    {
        if (!uncolored[i])
            continue;
        
        int incidence = 0;
        for (int j = 0; j < vertices; j++)
        {
            if (graph[i][j] > 0)
                incidence++;
        }
        
        if (incidence > maxIncidence)
        {
            maxIncidence = incidence;
            selectedNode = i;
        }
    }
    return selectedNode;
}

// Incidence Degree Ordering (IDO) Graph Coloring Algorithm
int idoGraphColoring(int **graph, int vertices)
{
    clock_t start_time = clock();

    idoSolution = (int *)malloc(vertices * sizeof(int));
    idoOrder = (int *)malloc(vertices * sizeof(int));
    bool *uncolored = (bool *)malloc(vertices * sizeof(bool));
    for (int i = 0; i < vertices; i++)
    {
        idoSolution[i] = -1;
        uncolored[i] = true;
    }

    int color = 1;
    int step = 0;
    while (1)
    {
        int node = findHighestIncidenceDegreeNode(graph, uncolored, vertices);
        if (node == -1)
            break;

        idoSolution[node] = color;
        uncolored[node] = false;
        idoOrder[step++] = node;

        for (int i = 0; i < vertices; i++)
        {
            if (uncolored[i])
            {
                bool conflict = false;
                for (int j = 0; j < vertices; j++)
                {
                    if (graph[i][j] > 0 && idoSolution[j] == color)
                    {
                        conflict = true;
                        break;
                    }
                }
                if (!conflict)
                {
                    idoSolution[i] = color;
                    uncolored[i] = false;
                    idoOrder[step++] = i;
                }
            }
        }
        color++;
    }


    free(uncolored);
    
    clock_t end_time = clock();
    double elapsed_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC * 1000;
    totalTime_ido += elapsed_time;
    printf("IDO completed in: %.2f ms\n", elapsed_time);
    
    int maxColor = 0;
    for (int i = 0; i < vertices; i++)
    {
        if (idoSolution[i] > maxColor)
            maxColor = idoSolution[i];
    }
    
    return maxColor;
}

// Function to retrieve the IDO solution
int *getIdoSolution() {
    return idoSolution;
}

// Function to retrieve the order in which nodes were colored
int *getIdoOrder() {
    return idoOrder;
}

// Function to print the total execution time
void printTotalTimeIDO()
{
    printf("Total execution time for IDO: %.2f ms\n", totalTime_ido);
}