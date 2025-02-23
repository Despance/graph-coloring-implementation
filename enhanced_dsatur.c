#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Function prototypes
void convertToWeightedGraph(int **graph, int vertices, int n);
void updateNodeWeights(int **graph, int *nodeWeights, int nodes, int n);
int newGraphColoring(int **graph, int *nodeWeights, int nodes, int n);
int enhancedDSatur(int **graph, int *nodeWeights, int vertices);
int colorGraph(int **graph, int *nodeWeights, int nodes);
void updateNeighboursWeights(int **graph, int *nodeWeights, int nodes, int currentNode);
void colorNodeFirstFit(int **graph, int *colors, bool *colored, int nodes, int currentNode);
int findHighestWeightedNode(int *nodeWeights, bool *colored, int vertices);
void printTotalTime();


// Global variable to store the total execution time
double totalTime = 0.0;

// OLD FUNCTION, DO NOT USE FOR FURTHER IMPLEMENTATIONS!!!
// USE updateNodeWeights INSTEAD!!!
// Function to convert the graph to a weighted graph
void convertToWeightedGraph(int **graph, int vertices, int n)
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

    // Initialize node values
    for (int i = 0; i < vertices; i++)
    {
        nodeValues[i] = 0;
    }

    // Iteration loop
    for (int iter = 0; iter < n; iter++)
    {
        printf("Iteration %d:\n", iter + 1);

        // Step 1: Calculate node degree values or update with current node weights
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

        // Output the node weights for this iteration
        printf("Node Weights:\n");
        for (int i = 0; i < vertices; i++)
        {
            printf("Node %d: %d\n", i + 1, nodeWeights[i]);
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

    // Time measurement end
    clock_t end_time = clock();
    double elapsed_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC * 1000; // Milisaniye cinsinden
    totalTime += elapsed_time;                                                       // Toplam süreye ekle

    printf("convertToWeightedGraph completed in: %.2f ms\n", elapsed_time);
}

void updateNodeWeights(int **graph, int *nodeWeights ,int nodes, int n){

    int *nodeValues = (int *)calloc(nodes, sizeof(int));          // Node degree values (init with 0)
    int **tempEdgeWeights = (int **)malloc(nodes * sizeof(int *)); // Temporary edge weights

    // Allocate memory for tempEdgeWeights
    for (int i = 0; i < nodes; i++){
        tempEdgeWeights[i] = (int *)malloc(nodes * sizeof(int));
    }

    
    // Iteration loop
    for (int iter = 0; iter < n; iter++){
        printf("Iteration %d:\n", iter + 1);

        // Step 1: Calculate node degree values or update with current node weights
        for (int i = 0; i < nodes; i++)
        {
            nodeValues[i] = 0;
            for (int j = 0; j < nodes; j++)
            {
                if (graph[i][j] > 0) // TODO: we may not need this check(if we don't have overflows etc.)
                {
                    nodeValues[i] += graph[i][j]; // Sum of edge weights connected to the node
                }
            }
        }

        // Step 2: Calculate edge weights and update adjacency matrix
        for (int i = 0; i < nodes; i++)
        {
            for (int j = 0; j < nodes; j++)
            {
                if (graph[i][j] > 0) // Edge exists
                {
                    tempEdgeWeights[i][j] = nodeValues[i] + nodeValues[j]; // Calculate edge weight
                    graph[i][j] = tempEdgeWeights[i][j]; // Update adjacency matrix with edge weight
                }
            }
        }

        // Step 3: Calculate node weights
        for (int i = 0; i < nodes; i++)
        {
            nodeWeights[i] = 0;
            for (int j = 0; j < nodes; j++)
            {
                if (graph[i][j] > 0) // Edge exists (TODO: we may not need this check)
                {
                    nodeWeights[i] += graph[i][j];
                }
            }
        }

        // Output the node weights for this iteration
        printf("Node Weights:\n");
        for (int i = 0; i < nodes; i++)
        {
            printf("Node %d: %d\n", i + 1, nodeWeights[i]);
        }


    }

    // Free dynamically allocated memory
    free(nodeValues);
    for (int i = 0; i < nodes; i++)
    {
        free(tempEdgeWeights[i]);
    }
    free(tempEdgeWeights);
}


// method to color the graph using First Fit and 2x the weights of the neighbours
int newGraphColoring(int **graph, int *nodeWeights, int nodes, int n){ // TODO: find a better name
    clock_t start_time;
    clock_t end_time;
    
    start_time = clock(); // Time measurement start
    updateNodeWeights(graph, nodeWeights, nodes, n); // Update node weights
    end_time = clock();// Time measurement end
    
    double elapsed_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC * 1000; // calculate time in miliseconds
    totalTime += elapsed_time;                                                       // Add to total time
    printf("updateNodeWeights completed in: %.2f ms\n", elapsed_time);
    

    start_time = clock(); // Start time for coloring
    int numOfColors = colorGraph(graph, nodeWeights, nodes); // Color the graph
    end_time = clock(); // End time for coloring
    
    elapsed_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC * 1000; // calculate time in miliseconds
    totalTime += elapsed_time;                                                       // Add to total time
    printf("colorGraph completed in: %.2f ms\n", elapsed_time);
    
    printf("enhancedDSatur completed in: %.2f ms\n", elapsed_time);
    return numOfColors;
}

// Coloring using First Fit and 2x the weights of the neighbours
int colorGraph(int **graph, int *nodeWeights, int nodes){
    // Start DSatur like coloring process
    int *colors = (int *)malloc(nodes * sizeof(int));
    bool *colored = (bool *)malloc(nodes * sizeof(bool));
    int numOfColors = -1;

    for (int i = 0; i < nodes; i++)
    {
        colors[i] = -1; // No color assigned initially
        colored[i] = false;
    }

    // Coloring process, First Fit and 2x the weights of the neighbours
    for(int i = 0; i< nodes;i++){
        int currentNode = findHighestWeightedNode(nodeWeights, colored, nodes);
        colorNodeFirstFit(graph, colors, colored, nodes, currentNode); // First Fit Coloring the highest weighted node
        updateNeighboursWeights(graph, nodeWeights, nodes, currentNode); // Update the weights of the neighbours(2x them)
    }
    
    for(int i = 0; i < nodes; i++){
        if(colored[i]){
            printf("Node %d: Color %d\n", i + 1, colors[i]);
            if(colors[i] > numOfColors){
                numOfColors = colors[i];
            }
        }
    }
    printf("Number of colors used: %d\n", numOfColors + 1);
    return numOfColors + 1;
}

// method to 2x the weights of the current node's neighbours
void updateNeighboursWeights(int **graph, int *nodeWeights, int nodes, int currentNode){
    for (int i = 0; i < nodes; i++){
        if (graph[currentNode][i] > 0){ // Check adjacency
            nodeWeights[i] += nodeWeights[i]; // Double the weight
        }
    }
}

// First Fit Colores the current node
void colorNodeFirstFit(int **graph, int *colors, bool *colored, int nodes, int currentNode){
    bool *usedColors = (bool *)calloc(nodes, sizeof(bool));
    for (int j = 0; j < nodes; j++)
    {
        if (graph[currentNode][j] > 0 && colored[j]) // Check adjacency and color status
        {
            if (colors[j] <= nodes){ // Check if the color is valid (TODO: we may not need this check)
                usedColors[colors[j]] = true; // Mark the color as used
            }
        }
    }

    int color = 0;
    while (color <= nodes && usedColors[color]){ // Find the first available color
        color++;
    }
    colors[currentNode] = color;
    colored[currentNode] = true;
    free(usedColors);
}


// Function to find the highest weighted uncolored node
int findHighestWeightedNode(int *nodeWeights, bool *colored, int vertices){
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


int enhancedDSatur(int **graph, int *nodeWeights, int vertices){
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
        free(usedColors);

        for (int i = 0; i < vertices; i++)
        {
            free(newGraph[i]);
        }
        free(newGraph);
        free(newWeights);
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
    totalTime += elapsed_time;                                                       // Toplam süreye ekle
    
    printf("enhancedDSatur completed in: %.2f ms\n", elapsed_time);
    
    return maxColor + 1;
}
    
   
   /*
   !!!Broken code, gives false results!!!

static int *weightForSort; // For qsort comparator

int compareDescending(const void *a, const void *b)
{
    int idxA = *(const int *)a;
    int idxB = *(const int *)b;
    return weightForSort[idxB] - weightForSort[idxA];
}

int enhancedDSatur(int **graph, int *nodeWeights, int vertices)
{
    // Time measurement start
    clock_t start_time = clock();

    // 1. Convert original graph to weighted graph
    convertToWeightedGraph(graph, vertices, 1);

    // 2. Sort nodes by descendant weight
    int *nodeOrder = (int *)malloc(vertices * sizeof(int));
    for (int i = 0; i < vertices; i++)
    {
        nodeOrder[i] = i;
    }
    weightForSort = nodeWeights;
    qsort(nodeOrder, vertices, sizeof(int), compareDescending);

    // Prepare coloring arrays
    int *colors = (int *)malloc(vertices * sizeof(int));
    bool *colored = (bool *)malloc(vertices * sizeof(bool));
    for (int i = 0; i < vertices; i++)
    {
        colors[i] = -1;
        colored[i] = false;
    }

    int maxColorUsed = -1;
    int index = 0;
    while (index < vertices)
    {
        // Try coloring the first uncolored node
        int firstNode = nodeOrder[index];
        if (colored[firstNode])
        {
            index++;
            continue;
        }

        // Attempt to color the first node with existing colors
        bool assignedFirst = false;
        for (int c = 0; c <= maxColorUsed; c++)
        {
            // Check adjacency for conflicts
            bool canUse = true;
            for (int j = 0; j < vertices; j++)
            {
                if (graph[firstNode][j] && colors[j] == c)
                {
                    canUse = false;
                    break;
                }
            }
            if (canUse)
            {
                colors[firstNode] = c;
                colored[firstNode] = true;
                assignedFirst = true;
                break;
            }
        }
        if (!assignedFirst)
        {
            // No existing color works, create a new color
            maxColorUsed++;
            colors[firstNode] = maxColorUsed;
            colored[firstNode] = true;
        }

        // Now enter the loop to color remaining nodes
        int loopIndex = index + 1;
        for (; loopIndex < vertices; loopIndex++)
        {
            int currentNode = nodeOrder[loopIndex];
            if (colored[currentNode])
                continue;

            bool assigned = false;
            for (int c = 0; c <= maxColorUsed; c++)
            {
                // Check adjacency
                bool canUse = true;
                for (int k = 0; k < vertices; k++)
                {
                    if (graph[currentNode][k] && colors[k] == c)
                    {
                        canUse = false;
                        break;
                    }
                }
                if (canUse)
                {
                    colors[currentNode] = c;
                    colored[currentNode] = true;
                    assigned = true;
                    break;
                }
            }
            if (!assigned)
            {
                // Can't color with existing colors, leave loop to rebuild graph
                break;
            }
        }

        // Create a new graph without colored nodes
        int **newGraph = (int **)malloc(vertices * sizeof(int *));
        for (int i = 0; i < vertices; i++)
        {
            newGraph[i] = (int *)malloc(vertices * sizeof(int));
            for (int j = 0; j < vertices; j++)
            {
                newGraph[i][j] = colored[i] ? 0 : (colored[j] ? 0 : graph[i][j]);
            }
        }
        convertToWeightedGraph(newGraph, vertices, 1);

        // Recompute node weights
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

        // Re-sort nodes by new weight
        weightForSort = newWeights;
        qsort(nodeOrder, vertices, sizeof(int), compareDescending);

        // Free temporary data
        for (int i = 0; i < vertices; i++)
        {
            free(newGraph[i]);
        }
        free(newGraph);
        free(newWeights);

        index = loopIndex;
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
    totalTime += elapsed_time;                                                       // Toplam süreye ekle

    printf("enhancedDSatur completed in: %.2f ms\n", elapsed_time);

    return maxColor + 1;
}
*/

// Function to print the total execution time
void printTotalTime()
{
    printf("Total execution time: %.2f ms\n", totalTime);
}
