#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Global variable to store the total execution time
double totalTime = 0.0;

// Function to convert the graph to a weighted graph
void convertToWeightedGraph(int **graph, int vertices)
{
    // Time measurement start
    clock_t start_time = clock();

    int *nodeValues = (int *)malloc(vertices * sizeof(int));          // Node degree values
    int **tempEdgeWeights = (int **)malloc(vertices * sizeof(int *)); // Temporary edge weights
    int *nodeWeights = (int *)malloc(vertices * sizeof(int));         // Final node weights

    // Allocate memory for tempEdgeWeights
    for (int i = 0; i < vertices; i++)
    {
        tempEdgeWeights[i] = (int *)malloc(vertices * sizeof(int));
    }

    // Step 1: Calculate node degree values
    for (int i = 0; i < vertices; i++)
    {
        nodeValues[i] = 0;
        for (int j = 0; j < vertices; j++)
        {
            if (graph[i][j] > 0)
            {
                nodeValues[i]++;
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

    // Output the node weights
    printf("Node Weights:\n");
    for (int i = 0; i < vertices; i++)
    {
        printf("Node %d: %d\n", i + 1, nodeWeights[i]);
    }

    // Free dynamically allocated memory
    free(nodeValues);
    free(nodeWeights);
    for (int i = 0; i < vertices; i++)
    {
        free(tempEdgeWeights[i]);
    }
    free(tempEdgeWeights);

    // Time measurement end
    clock_t end_time = clock();
    double elapsed_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC * 1000; // Milisaniye cinsinden
    totalTime += elapsed_time; // Toplam süreye ekle

    printf("convertToWeightedGraph completed in: %.2f ms\n", elapsed_time);
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

// Function to calculate saturation degree for uncolored nodes
int calculateSaturation(int **graph, int *colors, int node, int vertices)
{
    bool *usedColors = (bool *)malloc(vertices * sizeof(bool));
    for (int i = 0; i < vertices; i++)
    {
        usedColors[i] = false;
    }

    for (int i = 0; i < vertices; i++)
    {
        if (graph[node][i] > 0 && colors[i] != -1)
        { // Adjacent and already colored
            usedColors[colors[i]] = true;
        }
    }

    int saturation = 0;
    for (int i = 0; i < vertices; i++)
    {
        if (usedColors[i])
        {
            saturation++;
        }
    }

    free(usedColors);
    return saturation;
}

// Function to find the next node to color
int findNextNode(int **graph, int *colors, int vertices, bool *colored)
{
    int maxSaturation = -1, selectedNode = -1;
    for (int i = 0; i < vertices; i++)
    {
        if (!colored[i])
        {
            int saturation = calculateSaturation(graph, colors, i, vertices);
            if (saturation > maxSaturation)
            {
                maxSaturation = saturation;
                selectedNode = i;
            }
        }
    }
    return selectedNode;
}

// Enhanced DSatur algorithm
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

    // Step 1: Pick the first node based on highest node weight
    int currentNode = findHighestWeightedNode(nodeWeights, colored, vertices);
    colors[currentNode] = 0; // Assign the first color
    colored[currentNode] = true;

    // Step 2: Color the rest of the nodes
    for (int step = 1; step < vertices; step++)
    {
        // Find the next node to color based on saturation degree
        currentNode = findNextNode(graph, colors, vertices, colored);

        // Assign the smallest available color
        bool *usedColors = (bool *)malloc(vertices * sizeof(bool));
        for (int i = 0; i < vertices; i++)
        {
            usedColors[i] = false;
        }

        for (int i = 0; i < vertices; i++)
        {
            if (graph[currentNode][i] > 0 && colors[i] != -1)
            { // Adjacent and already colored
                usedColors[colors[i]] = true;
            }
        }

        int color = 0;
        while (usedColors[color])
        {
            color++;
        }

        colors[currentNode] = color; // Assign the color
        colored[currentNode] = true;

        free(usedColors);
    }

    // Print the coloring result
    printf("Node Colors:\n");
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
    totalTime += elapsed_time; // Toplam süreye ekle

    printf("enhancedDSatur completed in: %.2f ms\n", elapsed_time);

    return maxColor + 1;
}

// Function to print the total execution time
void printTotalTime()
{
    printf("Total execution time: %.2f ms\n", totalTime);
}
