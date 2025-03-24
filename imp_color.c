#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// Function prototypes
void updateNodeWeights(int **graph, int *nodeWeights, int nodes, int n);
void calculateImpColor(int **graph, int *nodeWeights, int nodes, int n);
void colorGraphImpRecalculate(int **graph, int *nodeWeights, int nodes, int n);
void colorGraphImpRecalculateWhenNeeded(int **graph, int *nodeWeights, int nodes, int n);
void colorGraphFlowerHerd(int **graph, int *nodeWeights, int nodes, int n);
void colorGraphImpMixDSatur(int **graph, int *nodeWeights, int nodes);
void updateNeighboursWeights(int **graph, int *nodeWeights, int nodes, int currentNode, int n);
void colorNodeFirstFit(int **graph, int *colors, bool *colored, int nodes, int currentNode);
int findHighestWeightedMarkedNode(int **graph, int *nodeWeights, bool *colored, bool *uncoloredNeighbours, int nodes);
void colorAndMarkNeighbours(int **graph, int nodes, int *solution, int *orderOfNodes, int currentNode, bool *colored, bool *uncoloredNeighbours, int *index);
int colorNodeAndNeighbors(int **graph, int **tempGraph, int *nodeWeights, int nodes, int *solution, int *orderOfNodes, int currentNode, bool *colored, int neighborDegree, int *index);
int markNeighbours(int **graph, int nodes, bool *colored, bool *uncoloredNeighbours, int currentNode);
int getPossibleColorFirstFit(int **graph, int *colors, bool *colored, int nodes, int currentNode);
int findHighestWeightedNode(int *nodeWeights, bool *colored, int vertices);
int findHighestWeightedNeighbourNode(int **graph, int *nodeWeights, bool *colored, int nodes, int currentNode);
void removeNode(int **graph, int nodes, int node);
void resetWeights(int **graph, int *nodeWeights, int nodes);
int getPossibleColorRandomFit(int **graph, int *colors, bool *colored, int nodes, int currentNode);
int getPossibleColorNextFit(int **graph, int *colors, bool *colored, int nodes, int currentNode);
int getPossibleColorBestFit(int **graph, int *colors, bool *colored, int nodes, int currentNode);

// Global variables

int *solution; // Solution array

int *orderOfNodes; // Order of nodes array

time_t exec_time_imp;

double totalTime_imp = 0.0; // Total execution time

void updateNodeWeights(int **graph, int *nodeWeights, int nodes, int n)
{

    int *nodeValues = (int *)calloc(nodes, sizeof(int));           // Node degree values (init with 0)
    int **tempEdgeWeights = (int **)malloc(nodes * sizeof(int *)); // Temporary edge weights

    // Allocate memory for tempEdgeWeights
    for (int i = 0; i < nodes; i++)
    {
        tempEdgeWeights[i] = (int *)malloc(nodes * sizeof(int));
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
                if (graph[i][j] > 0) // Edge exists (TODO: we may not need this check)
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

// method to color the graph using First Fit and 2x the weights of the neighbours
void calculateImpColor(int **graph, int *nodeWeights, int nodes, int n)
{
    exec_time_imp = clock();

    /*
    updateNodeWeights(graph, nodeWeights, nodes, n); // Update node weights
    colorGraphImpMixDSatur(graph, nodeWeights, nodes); // Color the graph using First Fit and 2x the weights of the neighbours
    */

    // colorGraphImpRecalculate(graph, nodeWeights, nodes, n); // Color the graph by First Fit, removal of colored nodes and recalculation of the node weights

    // Color the graph by First Fit, removal of colored nodes and recalculation of the node weights when a new color is needed
    colorGraphImpRecalculateWhenNeeded(graph, nodeWeights, nodes, n);

    // colorGraphFlowerHerd(graph, nodeWeights, nodes, n);
    // colorGraphFlowerImpRecalculateWhenNeeded(graph, nodeWeights, nodes, n, 2);

    totalTime_imp = ((double)(clock() - exec_time_imp)) / CLOCKS_PER_SEC * 1000;
}


// Coloring by removal of colored nodes and recalculation of the node weights only when a new color is needed
void colorGraphImpRecalculateWhenNeeded(int **graph, int *nodeWeights, int nodes, int n)
{
    solution = (int *)calloc(nodes, sizeof(int)); // Solution array, 0 is no color assigned
    orderOfNodes = (int *)malloc(nodes * sizeof(int));
    bool *colored = (bool *)calloc(nodes, sizeof(bool));
    int **tempGraph = (int **)malloc(nodes * sizeof(int *)); // Temporary graph
    int maxColor = 0;                                        // Maximum color used
    bool firstNode = true;                                   // Boolean to determine whether this is the first node after graph calculation

    // Allocate memory for tempGraph
    for (int i = 0; i < nodes; i++)
    {
        tempGraph[i] = (int *)malloc(nodes * sizeof(int));
    }

    // copy graph to temp graph
    for (int i = 0; i < nodes; i++)
    {
        memcpy(tempGraph[i], graph[i], nodes * sizeof(int));
    }

    updateNodeWeights(tempGraph, nodeWeights, nodes, n); // calculate the initial node weights
    //int previousNode = -1; // for best neighbour coloring
    // Coloring process, First Fit and recalculation of the node weights when a new color is needed
    for (int i = 0; i < nodes; i++)
    {
        int currentNode = findHighestWeightedNode(nodeWeights, colored, nodes);
        //currentNode = (previousNode != -1)? findHighestWeightedNeighbourNode(graph, nodeWeights, colored, nodes, previousNode) : currentNode; // for best neighbour coloring
        orderOfNodes[i] = currentNode;
        int color = getPossibleColorFirstFit(graph, solution, colored, nodes, currentNode); // Get potential First Fit color

        // int color = getPossibleColorRandomFit(graph, solution, colored, nodes, currentNode); // Get potential First Fit color

        // int color = getPossibleColorNextFit(graph, solution, colored, nodes, currentNode); // Get potential First Fit color

        // int color = getPossibleColorBestFit(graph, solution, colored, nodes, currentNode); // Get potential First Fit color
        if (color > maxColor && !firstNode)
        { // If a new color is needed, recalculate the graph
            firstNode = true;
            // removeNode(tempGraph, nodes, currentNode); // Remove the colored node
            resetWeights(tempGraph, nodeWeights, nodes);         // Reset the node weights
            updateNodeWeights(tempGraph, nodeWeights, nodes, n); // Update the node weights
            i--;                                                 // Decrement the counter to recalculate the current node
            continue;
        }
        else if (firstNode)
        { // If this is the first node after graph calculation, update the maxColor
            maxColor = color;
        }

        firstNode = false;
        solution[currentNode] = color;
        colored[currentNode] = true;
        removeNode(tempGraph, nodes, currentNode); // Remove the colored node
        //previousNode = currentNode; // for best neighbour coloring
    }

    // free memory for tempGraph
    for (int i = 0; i < nodes; i++)
    {
        free(tempGraph[i]);
    }
    free(tempGraph);
    free(colored);
}

// Coloring by removal of colored nodes and recalculation of the node weights
void colorGraphImpRecalculate(int **graph, int *nodeWeights, int nodes, int n)
{                                                 // TODO: find better names for these functions... :'(
    solution = (int *)calloc(nodes, sizeof(int)); // Solution array, 0 is no color assigned
    orderOfNodes = (int *)malloc(nodes * sizeof(int));
    bool *colored = (bool *)calloc(nodes, sizeof(bool));
    int **tempGraph = (int **)malloc(nodes * sizeof(int *)); // Temporary graph

    // Allocate memory for tempGraph
    for (int i = 0; i < nodes; i++)
    {
        tempGraph[i] = (int *)malloc(nodes * sizeof(int));
    }

    // copy graph to temp graph
    for (int i = 0; i < nodes; i++)
    {
        memcpy(tempGraph[i], graph[i], nodes * sizeof(int));
    }

    for (int i = 0; i < nodes; i++)
    {
        updateNodeWeights(tempGraph, nodeWeights, nodes, n); // Update the node weights
        int currentNode = findHighestWeightedNode(nodeWeights, colored, nodes);
        orderOfNodes[i] = currentNode;
        colorNodeFirstFit(graph, solution, colored, nodes, currentNode); // First Fit Coloring the highest weighted node

        removeNode(tempGraph, nodes, currentNode);   // Remove the colored node
        resetWeights(tempGraph, nodeWeights, nodes); // Reset the node weights
    }

    // free memory for tempGraph
    for (int i = 0; i < nodes; i++)
    {
        free(tempGraph[i]);
    }
    free(tempGraph);
    free(colored);
}


// Coloring by removal of colored nodes and recalculation of the node weights only when a new color is needed
// This is the importance recalculate when needed with the "Flower" coloring 
// instead of just coloring the highest weighted node, it colors its n degree neighbours too 
void colorGraphFlowerImpRecalculateWhenNeeded(int **graph, int *nodeWeights, int nodes, int n, int neighborDegree)
{
    solution = (int *)calloc(nodes, sizeof(int)); // Solution array, 0 is no color assigned
    orderOfNodes = (int *)malloc(nodes * sizeof(int));
    bool *colored = (bool *)calloc(nodes, sizeof(bool));
    int **tempGraph = (int **)malloc(nodes * sizeof(int *)); // Temporary graph
    int maxColor = 0;                                        // Maximum color used
    bool firstNode = true;                                   // Boolean to determine whether this is the first node after graph calculation

    // Allocate memory for tempGraph
    for (int i = 0; i < nodes; i++)
    {
        tempGraph[i] = (int *)malloc(nodes * sizeof(int));
    }

    // copy graph to temp graph
    for (int i = 0; i < nodes; i++)
    {
        memcpy(tempGraph[i], graph[i], nodes * sizeof(int));
    }

    updateNodeWeights(tempGraph, nodeWeights, nodes, n); // calculate the initial node weights

    int i = 0;
    int *index = &i;
    // Coloring process, First Fit and recalculation of the node weights when a new color is needed
    for (; i < nodes; i++)
    {
        int currentNode = findHighestWeightedNode(nodeWeights, colored, nodes);
        int color = getPossibleColorFirstFit(graph, solution, colored, nodes, currentNode); // Get potential First Fit color

        if (color > maxColor && !firstNode)
        { // If a new color is needed, recalculate the graph
            firstNode = true;
            // removeNode(tempGraph, nodes, currentNode); // Remove the colored node
            resetWeights(tempGraph, nodeWeights, nodes);         // Reset the node weights
            updateNodeWeights(tempGraph, nodeWeights, nodes, n); // Update the node weights
            i--;                                                 // Decrement the counter to recalculate the current node
            continue;
        }
        else if (firstNode)
        { // If this is the first node after graph calculation, update the maxColor
            maxColor = color;
        }

        firstNode = false;
        // Color the current node and its n.th degree neighbours(colors from inner to outer)
        color = colorNodeAndNeighbors(graph, tempGraph, nodeWeights, nodes, solution, orderOfNodes, currentNode, colored, neighborDegree, index);
        if (color > maxColor)
        {
            maxColor = color;
        }
        removeNode(tempGraph, nodes, currentNode); // Remove the colored node
    }

    // free memory for tempGraph
    for (int i = 0; i < nodes; i++)
    {
        free(tempGraph[i]);
    }
    free(tempGraph);
    free(colored);
}




// THIS DOES NOT WORK VERY WELL (BUT WORKS)
// Thid firstly colors the best node, then colors the neighbours of that node
// then it marks the neighbours of the neighbours as uncolored
// then it colors the highest weighted marked node
// then colors its neighbours and marks their neighbours as uncolored and so on
// This is the Flower Herd algorithm (found by muho, but does not work very well)
void colorGraphFlowerHerd(int **graph, int *nodeWeights, int nodes, int n)
{
    solution = (int *)calloc(nodes, sizeof(int)); // Solution array, 0 is no color assigned
    orderOfNodes = (int *)malloc(nodes * sizeof(int));
    bool *colored = (bool *)calloc(nodes, sizeof(bool));
    int **tempGraph = (int **)malloc(nodes * sizeof(int *)); // Temporary graph
    bool *uncoloredNeighbours = (bool *)calloc(nodes, sizeof(bool));

    // Allocate memory for tempGraph
    for (int i = 0; i < nodes; i++)
    {
        tempGraph[i] = (int *)malloc(nodes * sizeof(int));
    }

    // copy graph to temp graph
    for (int i = 0; i < nodes; i++)
    {
        memcpy(tempGraph[i], graph[i], nodes * sizeof(int));
    }

    updateNodeWeights(tempGraph, nodeWeights, nodes, n); // Update the node weights

    int i = 0;
    int *index = &i;
    for (; i < nodes; i++)
    {
        int currentNode = findHighestWeightedMarkedNode(graph, nodeWeights, colored, uncoloredNeighbours, nodes);
        colorNodeFirstFit(graph, solution, colored, nodes, currentNode); // First Fit Coloring the current node
        orderOfNodes[i] = currentNode;
        uncoloredNeighbours[currentNode] = false; // Mark as colored
        colorAndMarkNeighbours(graph, nodes, solution, orderOfNodes, currentNode, colored, uncoloredNeighbours, index);

    }

    // free memory for tempGraph
    for (int i = 0; i < nodes; i++)
    {
        free(tempGraph[i]);
    }
    free(tempGraph);
    free(colored);
}

// Color the neighbours of the current node and mark their neighbours as uncolored
void colorAndMarkNeighbours(int **graph, int nodes, int *solution, int *orderOfNodes, int currentNode, bool *colored, bool *uncoloredNeighbours, int *index)
{   

    for (int i = 0; i < nodes; i++)
    {
        if (graph[currentNode][i] > 0 && !colored[i])
        {
            colorNodeFirstFit(graph, solution, colored, nodes, i); // First Fit Coloring the neighbours
            orderOfNodes[*index] = i;
            *index = *index + 1;
            uncoloredNeighbours[i] = false; // Mark the neighbour as colored
            markNeighbours(graph, nodes, colored, uncoloredNeighbours, i);

        }
    }
}

// Marke the uncolored neighbours of the current node
int markNeighbours(int **graph, int nodes, bool *colored, bool *uncoloredNeighbours, int currentNode)
{
    int counter = 0;
    for (int i = 0; i < nodes; i++)
    {
        if (graph[currentNode][i] > 0 && !colored[i])
        {
            uncoloredNeighbours[i] = true;
            counter++;
        }
    }
    return counter;

}

int findHighestWeightedMarkedNode(int **graph, int *nodeWeights, bool *colored, bool *uncoloredNeighbours, int nodes)
{
    int maxWeight = -1, selectedNode = -1;
    for (int i = 0; i < nodes; i++)
    {
        if (uncoloredNeighbours[i] && nodeWeights[i] > maxWeight)
        {
            maxWeight = nodeWeights[i];
            selectedNode = i;
        }
    }
    if(selectedNode == -1){
        selectedNode = findHighestWeightedNode(nodeWeights, colored, nodes);
    }
    return selectedNode;
}

// returns the highest color it assigns
int colorNodeAndNeighbors(int **graph, int **tempGraph, int *nodeWeights, int nodes, int *solution, int *orderOfNodes, int currentNode, bool *colored, int neighborDegree, int *index){
    bool *toBeColored = (bool *)calloc(nodes, sizeof(bool));
    bool *toBeNextColored = (bool *)calloc(nodes, sizeof(bool));
    int remainingToBeColored = 0;
    int remainingToBeNextColored = 0;
    int maxColor = 0;
    int color = 0;

    toBeColored[currentNode] = true;
    remainingToBeColored++;

    // color the current node and its n degree neighbours
    for(int n = 0; n<=neighborDegree; n++){
        
        for(int i = 0; i<nodes; i++){
            if(toBeColored[i]){
                color = getPossibleColorFirstFit(graph, solution, colored, nodes, i); // Get potential First Fit color
                solution[i] = color;
                colored[i] = true;
                orderOfNodes[*index] = i;
                *index = *index + 1;
                if(color > maxColor){
                    maxColor = color;
                }
                int count = markNeighbours(graph, nodes, colored, toBeNextColored, i); // mark the neighbours, so they will be colored next
                remainingToBeNextColored += count;
                removeNode(tempGraph, nodes, i); // Remove the colored node
                toBeColored[i] = false;
                remainingToBeColored--;
            }
        }
        // copy the next colored to the current colored
        for(int i = 0; i<nodes; i++){
            toBeColored[i] = toBeNextColored[i];
            toBeNextColored[i] = false;
        }
        remainingToBeColored = remainingToBeNextColored;
        remainingToBeNextColored = 0;
    }

    free(toBeColored);
    return maxColor;
}



// Returns the highest weighted neighbour node
int findHighestWeightedNeighbourNode(int **graph, int *nodeWeights, bool *colored, int nodes, int currentNode){
    int maxWeight = -1, selectedNode = -1;
    for (int i = 0; i < nodes; i++)
    {   
        if(graph[currentNode][i] > 0 && !colored[i] && nodeWeights[i] > maxWeight){
            maxWeight = nodeWeights[i];
            selectedNode = i;
        }
    }
    if(selectedNode == -1){
        selectedNode = findHighestWeightedNode(nodeWeights, colored, nodes);
    }
    return selectedNode;

}

// function to remove a node from the graph by setting the according adjacency matrix values to 0
void removeNode(int **graph, int nodes, int node)
{
    for (int i = 0; i < nodes; i++)
    {
        graph[node][i] = 0;
        graph[i][node] = 0;
    }
}

int getPossibleColorNextFit(int **graph, int *colors, bool *colored, int nodes, int currentNode)
{
    bool *usedColors = (bool *)calloc(nodes, sizeof(bool));
    int *colorCounts = (int *)calloc(nodes, sizeof(int));

    // Check neighbor colors and count usage of all colors
    for (int i = 0; i < nodes; i++)
    {
        // Mark colors used by neighbors as unavailable
        if (graph[currentNode][i] > 0 && colored[i])
        {
            usedColors[colors[i] - 1] = true;
        }

        // Count usage of each color in the graph
        if (colored[i] && colors[i] > 0)
        {
            colorCounts[colors[i] - 1]++;
        }
    }

    // First check if there are already-used colors available
    int selectedColor = -1;
    int minUsage = nodes + 1; // Start with a value higher than possible node count

    // Prioritize using an existing color with the least usage
    for (int c = 0; c < nodes; c++)
    {
        // If this color is used in the graph but not by neighbors
        if (colorCounts[c] > 0 && !usedColors[c])
        {
            if (colorCounts[c] < minUsage)
            {
                minUsage = colorCounts[c];
                selectedColor = c + 1; // Convert from 0-indexed to 1-indexed
            }
        }
    }

    // If no existing color is available, find the first unused color
    if (selectedColor == -1)
    {
        for (int c = 0; c < nodes; c++)
        {
            if (!usedColors[c])
            {
                selectedColor = c + 1;
                break;
            }
        }
    }

    free(usedColors);
    free(colorCounts);
    return selectedColor;
}

int getPossibleColorBestFit(int **graph, int *colors, bool *colored, int nodes, int currentNode)
{
    // Track colors used by neighbors (these are unavailable)
    bool *usedColors = (bool *)calloc(nodes, sizeof(bool));
    // Track colors already used somewhere in the graph
    bool *existingColors = (bool *)calloc(nodes, sizeof(bool));
    // Track count of each color used in the graph
    int *colorUsageCount = (int *)calloc(nodes, sizeof(int));

    // Count existing colors and identify colors unavailable due to neighbors
    for (int i = 0; i < nodes; i++)
    {
        if (colored[i])
        {
            int colorIndex = colors[i] - 1;
            existingColors[colorIndex] = true;
            colorUsageCount[colorIndex]++;

            // If this is a neighbor, mark color as unavailable
            if (graph[currentNode][i] > 0)
            {
                usedColors[colorIndex] = true;
            }
        }
    }

    // First check: Try to reuse an existing color to avoid introducing new colors
    int bestExistingColor = 0;
    bool foundExistingColor = false;
    int bestUsageCount = 0;

    for (int c = 0; c < nodes; c++)
    {
        // If color exists in graph but not used by neighbors
        if (existingColors[c] && !usedColors[c])
        {
            // Prefer the most frequently used color to consolidate
            if (!foundExistingColor || colorUsageCount[c] > bestUsageCount)
            {
                bestExistingColor = c + 1;
                bestUsageCount = colorUsageCount[c];
                foundExistingColor = true;
            }
        }
    }

    // If we found an existing color we can reuse, return it
    if (foundExistingColor)
    {
        free(usedColors);
        free(existingColors);
        free(colorUsageCount);
        return bestExistingColor;
    }

    // Second check: We need to introduce a new color
    // For each uncolored neighbor, calculate its saturation degree
    // (how many colors are already blocked for it)

    // Track which neighbors have the highest saturation
    int *neighborSaturation = (int *)calloc(nodes, sizeof(int));
    int *colorImpact = (int *)calloc(nodes, sizeof(int));

    for (int neighbor = 0; neighbor < nodes; neighbor++)
    {
        // Only consider uncolored neighbors
        if (graph[currentNode][neighbor] > 0 && !colored[neighbor])
        {
            // Count how many colors are unavailable to this neighbor
            bool *neighborForbiddenColors = (bool *)calloc(nodes, sizeof(bool));
            int saturation = 0;

            for (int n2 = 0; n2 < nodes; n2++)
            {
                if (graph[neighbor][n2] > 0 && colored[n2])
                {
                    int colorIdx = colors[n2] - 1;
                    if (!neighborForbiddenColors[colorIdx])
                    {
                        neighborForbiddenColors[colorIdx] = true;
                        saturation++;
                    }
                }
            }

            neighborSaturation[neighbor] = saturation;
            free(neighborForbiddenColors);
        }
    }

    // Calculate the impact of using each potential new color
    for (int c = 0; c < nodes; c++)
    {
        // Skip colors that are already used by neighbors
        if (usedColors[c])
            continue;

        // For each uncolored neighbor, see if using this color would reduce its options
        for (int neighbor = 0; neighbor < nodes; neighbor++)
        {
            if (graph[currentNode][neighbor] > 0 && !colored[neighbor])
            {
                // Check if this neighbor has a colored neighbor using color c+1
                bool hasConflict = false;
                for (int n2 = 0; n2 < nodes; n2++)
                {
                    if (graph[neighbor][n2] > 0 && colored[n2] && colors[n2] == c + 1)
                    {
                        hasConflict = true;
                        break;
                    }
                }

                if (hasConflict)
                {
                    // Weight the impact by the neighbor's saturation - more impact on highly saturated neighbors
                    colorImpact[c] += (neighborSaturation[neighbor] + 1);
                }
            }
        }
    }

    // Find the color with minimal impact
    int bestColor = -1;
    int minImpact = nodes * nodes;

    for (int c = 0; c < nodes; c++)
    {
        if (!usedColors[c] && colorImpact[c] < minImpact)
        {
            minImpact = colorImpact[c];
            bestColor = c + 1;
        }
    }

    // If no color found (shouldn't happen), use first fit as fallback
    if (bestColor == -1)
    {
        for (int c = 0; c < nodes; c++)
        {
            if (!usedColors[c])
            {
                bestColor = c + 1;
                break;
            }
        }
    }

    free(usedColors);
    free(existingColors);
    free(colorUsageCount);
    free(neighborSaturation);
    free(colorImpact);

    return bestColor;
}

int getPossibleColorRandomFit(int **graph, int *colors, bool *colored, int nodes, int currentNode)
{
    bool *usedColors = (bool *)calloc(nodes, sizeof(bool));
    bool *globalUsedColors = (bool *)calloc(nodes, sizeof(bool));

    // Identify colors used by neighbors
    for (int i = 0; i < nodes; i++)
    {
        if (graph[currentNode][i] > 0 && colored[i]) // Check adjacency and color status
        {
            usedColors[colors[i] - 1] = true; // Mark the color as used by neighbors
        }

        // Track all colors used in the graph
        if (colored[i] && colors[i] > 0)
        {
            globalUsedColors[colors[i] - 1] = true;
        }
    }

    // Create array of available colors that are already used elsewhere
    int *availableExistingColors = (int *)calloc(nodes, sizeof(int));
    int existingColorCount = 0;

    for (int i = 0; i < nodes; i++)
    {
        // If color is used in graph but not by neighbors
        if (globalUsedColors[i] && !usedColors[i])
        {
            availableExistingColors[existingColorCount++] = i + 1;
        }
    }

    int color;

    srand(time(NULL)); // Seed the random number generator with the current time
    if (existingColorCount > 0)
    {
        color = availableExistingColors[rand() % existingColorCount];
    }
    else
    {
        // No existing colors available, find first unused color
        color = 0;
        while (color < nodes && usedColors[color])
        {
            color++;
        }
        color++; // Convert from 0-indexed to 1-indexed
    }

    free(availableExistingColors);
    free(globalUsedColors);
    free(usedColors);
    return color;
}
// function to reset the node and edge weights
void resetWeights(int **graph, int *nodeWeights, int nodes)
{
    // Reset the node weights
    memset(nodeWeights, 0, nodes * sizeof(int));
    // reset the edge weights
    for (int i = 0; i < nodes; i++)
    {
        for (int j = 0; j < nodes; j++)
        {
            if (graph[i][j] > 0)
            {
                graph[i][j] = 1;
            }
        }
    }
}

// SEEMS TO BE WORKING BUT WORSE THAN RECALCULATE
// ADVISE: USE RECALCULATE INSTEAD
// Coloring using First Fit and 2x the weights of the neighbours
void colorGraphImpMixDSatur(int **graph, int *nodeWeights, int nodes)
{ // TODO: find better names for these functions... :'(
    // Start DSatur like coloring process
    solution = (int *)calloc(nodes, sizeof(int)); // Solution array, 0 is no color assigned
    orderOfNodes = (int *)malloc(nodes * sizeof(int));
    bool *colored = (bool *)malloc(nodes * sizeof(bool));

    for (int i = 0; i < nodes; i++)
    {
        solution[i] = -1; // No color assigned initially
        colored[i] = false;
    }

    // Coloring process, First Fit and 2x the weights of the neighbours
    for (int i = 0; i < nodes; i++)
    {
        int currentNode = findHighestWeightedNode(nodeWeights, colored, nodes);
        orderOfNodes[i] = currentNode;
        colorNodeFirstFit(graph, solution, colored, nodes, currentNode);    // First Fit Coloring the highest weighted node
        updateNeighboursWeights(graph, nodeWeights, nodes, currentNode, 2); // Update the weights of the neighbours(2x them)
    }

    free(colored);

    return;
}

// method to n times the weights of the current node's neighbours
void updateNeighboursWeights(int **graph, int *nodeWeights, int nodes, int currentNode, int n)
{
    for (int i = 0; i < nodes; i++)
    {
        if (graph[currentNode][i] > 0)
        {                                        // Check adjacency
            nodeWeights[i] = n * nodeWeights[i]; // n times the weight
        }
    }
}

// First Fit Colores the current node
void colorNodeFirstFit(int **graph, int *colors, bool *colored, int nodes, int currentNode)
{
    bool *usedColors = (bool *)calloc(nodes, sizeof(bool));

    for (int i = 0; i < nodes; i++)
    {
        if (graph[currentNode][i] > 0 && colored[i]) // Check adjacency and color status
        {
            usedColors[colors[i] - 1] = true; // Mark the color as used(-1 for indexing reasons, color 1 is at index 0)
        }
    }

    int color = 0;
    while (color <= nodes && usedColors[color])
    { // Find the first available color
        color++;
    }

    colors[currentNode] = color + 1; // Assign the color(+1 because 0 is no color assigned)
    colored[currentNode] = true;
    free(usedColors);
}

// Function to return a possible color for the current node using First Fit
int getPossibleColorFirstFit(int **graph, int *colors, bool *colored, int nodes, int currentNode)
{
    bool *usedColors = (bool *)calloc(nodes, sizeof(bool));

    for (int i = 0; i < nodes; i++)
    {
        if (graph[currentNode][i] > 0 && colored[i]) // Check adjacency and color status
        {
            usedColors[colors[i] - 1] = true; // Mark the color as used(-1 for indexing reasons, color 1 is at index 0)
        }
    }

    int color = 0;
    while (color <= nodes && usedColors[color])
    { // Find the first available color
        color++;
    }

    free(usedColors);
    return color + 1; // Return the color(+1 because 0 is no color assigned)
}

// Function to find the highest weighted uncolored node
int findHighestWeightedNode(int *nodeWeights, bool *colored, int nodes)
{
    int maxWeight = -1, selectedNode = -1;
    for (int i = 0; i < nodes; i++)
    {

        if (!colored[i] && nodeWeights[i] > maxWeight)
        {
            maxWeight = nodeWeights[i];
            selectedNode = i;
        }
    }
    return selectedNode;
}

// Getters for the solution and order of nodes

// Get the solution array, which contains the color assignments
// 0 means no color assigned
// 1,2,3,... are the colors
// The index of the array is the node number
int *getSolution()
{
    return solution;
}

// Get the order of nodes array
// The index of the array is the order of the node
// The value of the array is the node number
int *getOrderOfNodes()
{
    return orderOfNodes;
}
