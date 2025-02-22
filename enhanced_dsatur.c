#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Global variable to store the total execution time
double totalTime = 0.0;

// Function to convert the graph to a weighted graph
void convertToWeightedGraph(int **graph, int vertices, int n)
{
    int *nodeValues = (int *)malloc(vertices * sizeof(int));          // Node degree values
    int **tempEdgeWeights = (int **)malloc(vertices * sizeof(int *)); // Temporary edge weights
    int *nodeWeights = (int *)malloc(vertices * sizeof(int));         // Final node weights

    // Allocate memory for tempEdgeWeights
    for (int i = 0; i < vertices; i++)
        tempEdgeWeights[i] = (int *)malloc(vertices * sizeof(int));

    // Initialize node values
    for (int i = 0; i < vertices; i++)
        nodeValues[i] = 0;

    // Iteration loop
    for (int iter = 0; iter < n; iter++)
    {
        for (int i = 0; i < vertices; i++)
        {
            nodeValues[i] = 0;
            for (int j = 0; j < vertices; j++)
            {
                if (graph[i][j] > 0)
                {
                    nodeValues[i] += graph[i][j]; // Sum of edge weights connected to the node
                }
            }
        }

        // Step 2: Calculate edge weights and update adjacency matrix
        for (int i = 0; i < vertices; i++)
        {
            for (int j = 0; j < vertices; j++)
            {
                if (graph[i][j] > 0) // Edge exists
                {
                    tempEdgeWeights[i][j] = nodeValues[i] + nodeValues[j];
                    graph[i][j] = tempEdgeWeights[i][j]; // Update adjacency matrix with edge weight
                }
            }
        }

        // Step 3: Calculate node weights
        for (int i = 0; i < vertices; i++)
        {
            nodeWeights[i] = 0;
            for (int j = 0; j < vertices; j++)
            {
                if (graph[i][j] > 0) // Edge exists
                {
                    nodeWeights[i] += graph[i][j];
                }
            }
        }
    }

    // Free dynamically allocated memory
    free(nodeValues);
    free(nodeWeights);
    for (int i = 0; i < vertices; i++)
    {
        free(tempEdgeWeights[i]);
    }
    free(tempEdgeWeights);
}

// Function to find the highest weighted uncolored node
int findHighestWeightedNode(int *nodeWeights, bool *colored, int vertices)
{
    int maxWeight = -1, selectedNode = -1;
    for (int i = 0; i < vertices; i++)
    {
        if (!colored[i] && nodeWeights[i] > maxWeight)
        {
            maxWeight = nodeWeights[i];
            selectedNode = i;
        }
    }
    return selectedNode;
}

int enhancedDSatur(int **graph, int *nodeWeights, int vertices)
{
    // Time measurement start
    clock_t start_time = clock();

    int *colors = (int *)malloc(vertices * sizeof(int));
    bool *colored = (bool *)malloc(vertices * sizeof(bool));

    for (int i = 0; i < vertices; i++)
    {
        colors[i] = -1; // No color assigned initially
        colored[i] = false;
    }

    int maxUsedColor = 0; // Track the highest color index

    for (int step = 0; step < vertices; step++)
    {
        // 1. Create a newGraph excluding already colored nodes
        int **newGraph = (int **)malloc(vertices * sizeof(int *));
        for (int i = 0; i < vertices; i++)
        {
            newGraph[i] = (int *)malloc(vertices * sizeof(int));
            for (int j = 0; j < vertices; j++)
            {
                newGraph[i][j] = colored[i] ? 0 : (colored[j] ? 0 : graph[i][j]);
            }
        }

        // 2. Convert to weighted graph
        convertToWeightedGraph(newGraph, vertices, 1);

        // 3. Recompute node weights
        int *newWeights = (int *)malloc(vertices * sizeof(int));
        for (int i = 0; i < vertices; i++)
        {
            newWeights[i] = 0;
            for (int j = 0; j < vertices; j++)
            {
                if (newGraph[i][j] > 0)
                {
                    newWeights[i] += newGraph[i][j];
                }
            }
        }

        // 4. Find highest weighted uncolored node
        int currentNode = -1, maxWeight = -1;
        for (int i = 0; i < vertices; i++)
        {
            if (!colored[i] && newWeights[i] > maxWeight)
            {
                maxWeight = newWeights[i];
                currentNode = i;
            }
        }

        // 5. Assign the first available color, or create a new one
        bool *usedColors = (bool *)calloc(maxUsedColor + 1, sizeof(bool));
        for (int i = 0; i < vertices; i++)
        {
            if (graph[currentNode][i] > 0 && colors[i] != -1)
            {
                if (colors[i] <= maxUsedColor)
                {
                    usedColors[colors[i]] = true;
                }
            }
        }
        int color = 0;
        while (color <= maxUsedColor && usedColors[color])
        {
            color++;
        }
        if (color > maxUsedColor)
        {
            maxUsedColor = color;
        }
        colors[currentNode] = color;
        colored[currentNode] = true;

        // Print which node is colored with which color
        printf("%d- Node %d: Color %d\n", step + 1, currentNode + 1, color);

        free(usedColors);

        for (int i = 0; i < vertices; i++)
        {
            free(newGraph[i]);
        }
        free(newGraph);
        free(newWeights);
    }

    // Print the coloring result
    printf("Enhanced Final Colors:\n");
    for (int i = 0; i < vertices; i++)
    {
        printf("Node %d: Color %d\n", i + 1, colors[i]);
    }

    // Find the number of colors used
    int maxColor = 0;
    for (int i = 0; i < vertices; i++)
    {
        if (colors[i] > maxColor)
        {
            maxColor = colors[i];
        }
    }

    free(colors);
    free(colored);

    // Time measurement end
    clock_t end_time = clock();
    double elapsed_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC * 1000; // Milisaniye cinsinden
    totalTime += elapsed_time;                                                       // Toplam süreye ekle

    printf("enhancedDSatur completed in: %.2f ms\n", elapsed_time);

    return maxColor + 1;
}

// This method is for verifying the coloring solution that returns the color assignment array.
int *enhancedDSaturSolution(int **graph, int *nodeWeights, int vertices)
{
    clock_t start_time = clock();

    int *colors = (int *)malloc(vertices * sizeof(int));
    bool *colored = (bool *)malloc(vertices * sizeof(bool));

    for (int i = 0; i < vertices; i++)
    {
        colors[i] = -1;
        colored[i] = false;
    }
    int maxUsedColor = 0;

    for (int step = 0; step < vertices; step++)
    {
        int **newGraph = (int **)malloc(vertices * sizeof(int *));
        for (int i = 0; i < vertices; i++)
        {
            newGraph[i] = (int *)malloc(vertices * sizeof(int));
            for (int j = 0; j < vertices; j++)
                newGraph[i][j] = colored[i] ? 0 : (colored[j] ? 0 : graph[i][j]);
        }
        convertToWeightedGraph(newGraph, vertices, 1);
        int *newWeights = (int *)malloc(vertices * sizeof(int));
        for (int i = 0; i < vertices; i++)
        {
            newWeights[i] = 0;
            for (int j = 0; j < vertices; j++)
            {
                if (newGraph[i][j] > 0)
                    newWeights[i] += newGraph[i][j];
            }
        }
        int currentNode = -1, maxWeight = -1;
        for (int i = 0; i < vertices; i++)
        {
            if (!colored[i] && newWeights[i] > maxWeight)
            {
                maxWeight = newWeights[i];
                currentNode = i;
            }
        }
        bool *usedColors = (bool *)calloc(maxUsedColor + 1, sizeof(bool));
        for (int i = 0; i < vertices; i++)
        {
            if (graph[currentNode][i] > 0 && colors[i] != -1)
            {
                if (colors[i] <= maxUsedColor)
                    usedColors[colors[i]] = true;
            }
        }
        int color = 0;
        while (color <= maxUsedColor && usedColors[color])
            color++;
        if (color > maxUsedColor)
            maxUsedColor = color;
        colors[currentNode] = color;
        colored[currentNode] = true;
        printf("%d- Node %d: Color %d\n", step + 1, currentNode + 1, color);
        free(usedColors);
        for (int i = 0; i < vertices; i++)
            free(newGraph[i]);
        free(newGraph);
        free(newWeights);
    }

    printf("Enhanced Final Colors:\n");
    for (int i = 0; i < vertices; i++)
        printf("Node %d: Color %d\n", i + 1, colors[i]);

    int maxColor = 0;
    for (int i = 0; i < vertices; i++)
    {
        if (colors[i] > maxColor)
            maxColor = colors[i];
    }

    clock_t end_time = clock();
    double elapsed_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC * 1000;
    printf("enhancedDSatur completed in: %.2f ms\n", elapsed_time);

    free(colored);
    return colors; // return the allocated colors array
}

// Function to print the total execution time
void printTotalTime()
{
    printf("Total execution time: %.2f ms\n", totalTime);
}
