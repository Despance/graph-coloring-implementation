#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

int *mixSolution;
int *mixOrder;

time_t exec_time_mix;
double totalTime_mix = 0.0;

int *getMixSolution()
{
    return mixSolution;
}

int *getMixOrderOfNodes()
{
    return mixOrder;
}

void findNodeWeights(int **graph, float *nodeWeights, int nodes, int n)
{
    float *nodeValues = (float *)calloc(nodes, sizeof(float));           // Node degree values (init with 0)
    float **tempEdgeWeights = (float **)malloc(nodes * sizeof(float *)); // Temporary edge weights

    // Allocate memory for tempEdgeWeights
    for (int i = 0; i < nodes; i++)
    {
        tempEdgeWeights[i] = (float *)malloc(nodes * sizeof(float));
    }

    // Iteration loop
    for (int iter = 0; iter < n; iter++)
    {
        // Step 1: Calculate node degree values or update with current node weights
        for (int i = 0; i < nodes; i++)
        {
            nodeValues[i] = 0;
            for (int j = 0; j < nodes; j++)
            {
                if (graph[i][j] > 0) // edge exists
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
                if (graph[i][j] > 0) // edge exists
                {
                    tempEdgeWeights[i][j] = nodeValues[i] + nodeValues[j]; // Calculate edge weight
                    graph[i][j] = tempEdgeWeights[i][j];                   // Update adjacency matrix with edge weight
                }
            }
        }

        // Step 3: Calculate node weights
        for (int i = 0; i < nodes; i++)
        {
            nodeWeights[i] = 0;
            for (int j = 0; j < nodes; j++)
            {
                if (graph[i][j] > 0) // edge exists
                {
                    nodeWeights[i] += graph[i][j];
                }
            }
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

// Function to find the vertex with the maximum saturation degree
int findMaxSaturationVertex_mix(int **graph, int vertices, int saturation[], bool colored[], int degree[], int k)
{
    int maxSaturation = -1;
    int maxDegree = -1;
    int vertex = -1;
    float maxWeight = -1.0;

    float *nodeWeights = (float *)calloc(vertices, sizeof(float)); // Node weights
    findNodeWeights(graph, nodeWeights, vertices, k);

    for (int i = 0; i < vertices; i++)
    {
        if (!colored[i])
        {
            // If current vertex has a higher saturation, update.
            if (saturation[i] > maxSaturation)
            {
                maxSaturation = saturation[i];
                maxDegree = degree[i];
                maxWeight = nodeWeights[i];
                vertex = i;
            }
            // If saturation is the same, check for higher degree.
            else if (saturation[i] == maxSaturation)
            {
                if (degree[i] > maxDegree)
                {
                    maxDegree = degree[i];
                    maxWeight = nodeWeights[i];
                    vertex = i;
                }
                // If degree is also the same, choose the one with highest weight.
                else if (degree[i] == maxDegree && nodeWeights[i] > maxWeight)
                {
                    maxWeight = nodeWeights[i];
                    vertex = i;
                }
            }
        }
    }
    free(nodeWeights);
    return vertex;
}
int findFirstColoredHighestWeightNode(int vertices, float *nodeWeights)
{
    int firstColoredNode = -1;
    float maxWeight = -1.0;

    for (int i = 0; i < vertices; i++)
    {
        if (mixSolution[i] > 0) // Check if the node is colored
        {
            if (firstColoredNode == -1 || nodeWeights[i] > maxWeight)
            {
                firstColoredNode = i;
                maxWeight = nodeWeights[i];
            }
        }
    }

    return firstColoredNode; // Returns the first colored node with the highest weight
}

// DSatur algorithm implementation
void dsatur_mix(int **graph, int vertices, int k)
{
    exec_time_mix = clock();

    mixSolution = (int *)calloc(vertices, sizeof(int));     // Array to store colors for each vertex
    mixOrder = (int *)calloc(vertices, sizeof(int));        // Array to store the order of nodes
    int *saturation = (int *)calloc(vertices, sizeof(int)); // Saturation degree for each vertex
    bool *colored = (bool *)calloc(vertices, sizeof(bool)); // Whether a vertex is colored
    int *degree = (int *)calloc(vertices, sizeof(int));     // Degree of each vertex

    // Calculate the degree of each vertex using > 0 since graph values may be > 1 after findNodeWeights
    for (int i = 0; i < vertices; i++)
    {
        for (int j = 0; j < vertices; j++)
        {
            if (graph[i][j] > 0)
            {
                degree[i]++;
            }
        }
    }
    float *nodeWeights = (float *)calloc(vertices, sizeof(float));
    findNodeWeights(graph, nodeWeights, vertices, k); // Compute node weights

    int resultNode = findFirstColoredHighestWeightNode(vertices, nodeWeights);
    printf("The first colored node with the highest weight is: %d\n", resultNode);

    free(nodeWeights);
    // Start coloring the graph
    for (int i = 0; i < vertices; i++)
    {
        int vertex = findMaxSaturationVertex_mix(graph, vertices, saturation, colored, degree, k);

        // Find the smallest available color for the chosen vertex
        bool *usedColors = (bool *)calloc(vertices + 1, sizeof(bool));
        for (int j = 0; j < vertices; j++)
        {
            if (graph[vertex][j] > 0 && mixSolution[j] > 0)
            {
                usedColors[mixSolution[j]] = true;
            }
        }

        int color = 1;
        while (usedColors[color])
        {
            color++;
        }

        mixSolution[vertex] = color; // Assign the color
        mixOrder[i] = vertex;        // Store the order of nodes
        colored[vertex] = true;

        free(usedColors); // Free the temporary array for used colors

        // Update saturation degrees of neighbors
        for (int j = 0; j < vertices; j++)
        {
            if (graph[vertex][j] > 0 && !colored[j])
            {
                bool *neighborColors = (bool *)calloc(vertices + 1, sizeof(bool));
                for (int m = 0; m < vertices; m++)
                {
                    if (graph[j][m] > 0 && mixSolution[m] > 0)
                    {
                        neighborColors[mixSolution[m]] = true;
                    }
                }

                int saturationCount = 0;
                for (int m = 1; m <= vertices; m++)
                {
                    if (neighborColors[m])
                    {
                        saturationCount++;
                    }
                }
                saturation[j] = saturationCount;

                free(neighborColors); // Free the temporary array for neighbor colors
            }
        }
    }

    totalTime_mix = (double)(clock() - exec_time_mix) / CLOCKS_PER_SEC;

    free(saturation);
    free(colored);
    free(degree);
}