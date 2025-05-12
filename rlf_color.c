#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>



// Function prototypes
void rlfColor(int **graph, int nodes);
void standartRLFColor(int **graph, int nodes, int *solution, int *orderOfNodes, bool *colored);
void importanceRLFColor(int **graph, int nodes, int *solution, int *orderOfNodes, bool *colored);
void colorMaxIndependentSetImportance(int **graph, int nodes, int *solution, int *orderOfNodes, bool *colored, int currentNode, int *nodeWeights);
void colorMaxIndependentSet(int **graph, int nodes, int *solution, int *orderOfNodes, bool *colored, int currentNode);
int findMaxIndependentNodeImportance(int **graph, int nodes, bool *colored, bool *currentSet, int *nodeWeights);
int findMaxIndependentNode(int **graph, int nodes, bool *colored, bool *currentSet);
int getNumOfNeighboursInSet(int **graph, int nodes, bool *currentSet, int currentNode);
int getNumOfNeighboursNotInSet(int **graph, int nodes, bool *currentSet, int currentNode);
int getNodeWithLargesNumberOfNeighbours(int **graph, int nodes, bool *colored);
int getNumOfNeighbours(int **graph, int nodes, int currentNode);
int findNextAvailableColor(int *solution, int nodes, bool *currentSet);
void removeNode(int **graph, int nodes, int node);

// Function prototypes for functions from imp_color.c, these should be in a seperate file in the future
void updateNodeWeights(int **graph, int *nodeWeights, int nodes, int n);
int findHighestWeightedNode(int *nodeWeights, bool *colored, int nodes);
void resetWeights(int **graph, int *nodeWeights, int nodes);

int *solution; // Solution array

int *orderOfNodes; // Order of nodes array
int orderOfNodesIndex = 0; // Index for order of nodes

time_t exec_time_imp;

double totalTime_imp = 0.0; // Total execution time

int **globalGraph; // Graph adjacency matrix

void rlfColor(int **graph, int nodes){
    
    exec_time_imp = clock(); // Start the timer

    globalGraph = graph; // Store the graph in a global variable

    // Initialize solution and orderOfNodes arrays
    solution = (int *)calloc(nodes, sizeof(int)); // Solution array
    orderOfNodes = (int *)malloc(nodes * sizeof(int));
    bool *colored = (bool *)calloc(nodes, sizeof(bool));
    int **tempGraph = (int **)malloc(nodes * sizeof(int *)); // Temporary graph
    // Allocate memory for tempGraph and copy it
    for (int i = 0; i < nodes; i++)
    {
        tempGraph[i] = (int *)malloc(nodes * sizeof(int));
        memcpy(tempGraph[i], graph[i], nodes * sizeof(int));
    }
    
    // Call the coloring function
    // standartRLFColor(tempGraph, nodes, solution, orderOfNodes, colored);
    importanceRLFColor(tempGraph, nodes, solution, orderOfNodes, colored); // Pass NULL for nodeWeights if not used

    // Free allocated memory
    free(colored);
    for (int i = 0; i < nodes; i++)
    {
        free(tempGraph[i]);
    }
    free(tempGraph);


    // Stop timer and calculate the total execution time
    totalTime_imp = ((double)(clock() - exec_time_imp)) / CLOCKS_PER_SEC * 1000;

}


void standartRLFColor(int **graph, int nodes, int *solution, int *orderOfNodes, bool *colored){


    // if node with largest num of neighbours can not be found(method returns -1), stop coloring
    int currentNode = 0;
    while(true){

        // STEP 1: Find the node with the largest number of neighbours as a starting point
        currentNode = getNodeWithLargesNumberOfNeighbours(graph, nodes, colored);
        if(currentNode == -1){
            break; // No more uncolored nodes
        }

        // STEP 2: find the maximum independent set in the graph. then color and remove these nodes
        colorMaxIndependentSet(graph, nodes, solution, orderOfNodes, colored, currentNode);
        
    }

}

void importanceRLFColor(int **graph, int nodes, int *solution, int *orderOfNodes, bool *colored){
    
    // if node with largest num of neighbours can not be found(method returns -1), stop coloring
    int currentNode = 0;

    int *nodeWeights = (int *)calloc(nodes, sizeof(int)); // Node weights array
    updateNodeWeights(graph, nodeWeights, nodes, 2); // Update node weights
    while(true){
        
        
        // STEP 1: Find the node with the highest weight as a starting point(this is the modified version)
        currentNode = getNodeWithLargesNumberOfNeighbours(graph, nodes, colored);

        // resetWeights(graph, nodeWeights, nodes); // Reset the node weights
        // updateNodeWeights(graph, nodeWeights, nodes, 2); // Update node weights
        // currentNode = findHighestWeightedNode(nodeWeights, colored, nodes);
        
        if(currentNode == -1){
            break; // No more uncolored nodes
        }

        // STEP 2: find the maximum independent set in the graph. then color and remove these nodes
        //colorMaxIndependentSet(graph, nodes, solution, orderOfNodes, colored, currentNode);
        colorMaxIndependentSetImportance(graph, nodes, solution, orderOfNodes, colored, currentNode, nodeWeights);
        
    }
    free(nodeWeights); // Free the node weights
}

void colorMaxIndependentSetImportance(int **graph, int nodes, int *solution, int *orderOfNodes, bool *colored, int currentNode, int *nodeWeights){
    // init the current set S as an array with calloc (TODO: in future use the same array just memset it to false)
    bool *currentSet = (bool *)calloc(nodes, sizeof(bool));
    currentSet[currentNode] = true; // add the current node to the set S
    

    // do this until no more nodes can be added to the set S
    while(true){

        // call findMaxIndependentNode, add it to the set S
        // int current = findMaxIndependentNode(graph, nodes, colored, currentSet); // original version
         int current = findMaxIndependentNodeImportance(graph, nodes, colored, currentSet, nodeWeights); // use the importance version
        if(current == -1){
            break; // No more uncolored nodes
        }
        // add the node to order of Nodes
        orderOfNodes[orderOfNodesIndex] = currentNode;
        orderOfNodesIndex++;
        currentSet[current] = true; // add the node to the set S
    }
    
    // color the nodes in set S
    
    int color = findNextAvailableColor(solution, nodes, currentSet);
    for(int i = 0; i < nodes; i++){
        if(currentSet[i]){
            solution[i] = color; // color the node
            colored[i] = true; // mark the node as colored
            // remove the nodes in set S from the graph(graph should be temporary)
            removeNode(graph, nodes, i); // remove the node from the graph
        }
    }
    

    free(currentSet); // free the current set S

}


// Finds the maximum independent set in the graph, called set S, and collors and removes it
// choose nodes by taking the node with the largest # of neighbours that are adjacent to the nodes in set S
// as a tie breaker, choose the node with the minimum # of neighbours not adjacent to the nodes set S
// a-b-c,
// a-d-c,
// a-b-e,
// a-d-e,
// f-g-e,  lets say a is in set S, others are not
// the neighbours of a are b and d, so we need to look at neighbours of them
// c has 2 neighbours(b and d) that are neighbours(b and d) to nodes in set S, e also has 2, so we need the tie breaker
// the node with the minimum # of neighbours not adjacent to the nodes in set S
// c has 0 neighbours not adjacent to nodes in set S, e has 1(node g) 
// so we take c
// and add it to the set S
// and remove it from the graph
// and repeat the process
// until no more nodes can be added to the set S
void colorMaxIndependentSet(int **graph, int nodes, int *solution, int *orderOfNodes, bool *colored, int currentNode){
    // init the current set S as an array with calloc (TODO: in future use the same array just memset it to false)
    bool *currentSet = (bool *)calloc(nodes, sizeof(bool));
    currentSet[currentNode] = true; // add the current node to the set S
    
    
    // do this until no more nodes can be added to the set S
    while(true){

        // call findMaxIndependentNode, add it to the set S
        int current = findMaxIndependentNode(graph, nodes, colored, currentSet);
        if(current == -1){
            break; // No more uncolored nodes
        }
        // add the node to order of Nodes
        orderOfNodes[orderOfNodesIndex] = currentNode;
        orderOfNodesIndex++;
        currentSet[current] = true; // add the node to the set S
    }

    
    // color the nodes in set S
    int color = findNextAvailableColor(solution, nodes, currentSet);
    for(int i = 0; i < nodes; i++){
        if(currentSet[i]){
            solution[i] = color; // color the node
            colored[i] = true; // mark the node as colored
            // remove the nodes in set S from the graph(graph should be temporary)
            removeNode(graph, nodes, i); // remove the node from the graph
        }
    }

    free(currentSet); // free the current set S

}

// given graph should be a temporary graph(with removed nodes)
// changed heuristic 2 to use weights of the nodes.
int findMaxIndependentNodeImportance(int **graph, int nodes, bool *colored, bool *currentSet, int *nodeWeights){

    // TODO: instead of allocating memory for this array every time, we can just reuse it
    // memset it to false
    bool *neighboursOfCurrentSet = (bool *)calloc(nodes, sizeof(bool)); 
    for(int i = 0; i < nodes; i++){
        if(currentSet[i]){
            for(int j = 0; j < nodes; j++){
                if(graph[i][j] > 0){
                    neighboursOfCurrentSet[j] = true;
                }
            }
        }
    }

    int maxNeighbours = 0;
    // int maxUnCommon = 0x7FFFFFFF; // init to largest value // this is for the og tie breaker
    // bool isMaxUnCommonSet = true; // for og tie breaker
    int nodeWithMaxNeighbours = -1;
    for(int i = 0; i < nodes; i++){
        if(colored[i] || neighboursOfCurrentSet[i] || currentSet[i]){
            continue; // Skip colored nodes
        }
        int current = getNumOfNeighboursInSet(graph, nodes, neighboursOfCurrentSet, i);
        
        // if the current node has more common neighbours than the max, update the max
        if(current > maxNeighbours){
            maxNeighbours = current;
            nodeWithMaxNeighbours = i;
            // isMaxUnCommonSet = false; // for og tie breaker
        }
        else if(current == maxNeighbours){
            // for the first node, if it's neighbour count is 0, it will try to access index -1, this if is to prevent that
            if(nodeWithMaxNeighbours == -1){ 
            maxNeighbours = current;
            nodeWithMaxNeighbours = i;
            continue; // skip the rest of the loop
            }

            int currentNodeWeight = nodeWeights[i];
            int maxNodeWeight = nodeWeights[nodeWithMaxNeighbours];
            if(currentNodeWeight > maxNodeWeight){
                maxNeighbours = current;
                nodeWithMaxNeighbours = i;
                // isMaxUnCommonSet = false; // for og tie breaker
            }
            /*
            else if(currentNodeWeight == maxNodeWeight){
                int currentUnCommon = getNumOfNeighboursNotInSet(graph, nodes, neighboursOfCurrentSet, i);
                if(!isMaxUnCommonSet){ // if max uncommon is not up to date, recompute  it
                    maxUnCommon = getNumOfNeighboursNotInSet(graph, nodes, neighboursOfCurrentSet, nodeWithMaxNeighbours);
                    isMaxUnCommonSet = true;
                }
                if(currentUnCommon < maxUnCommon){
                    maxNeighbours = current;
                    nodeWithMaxNeighbours = i;
                    maxUnCommon = currentUnCommon;
                }
            }
            */
            
        }
    }

    free(neighboursOfCurrentSet);
    return nodeWithMaxNeighbours;
}


// given graph should be a temporary graph(with removed nodes)
int findMaxIndependentNode(int **graph, int nodes, bool *colored, bool *currentSet){

    // TODO: instead of allocating memory for this array every time, we can just reuse it
    // memset it to false
    bool *neighboursOfCurrentSet = (bool *)calloc(nodes, sizeof(bool)); 
    for(int i = 0; i < nodes; i++){
        if(currentSet[i]){
            for(int j = 0; j < nodes; j++){
                if(graph[i][j] > 0){
                    neighboursOfCurrentSet[j] = true;
                }
            }
        }
    }

    int maxNeighbours = 0;
    int maxUnCommon = 0x7FFFFFFF; // init to largest value
    bool isMaxUnCommonSet = true;
    int nodeWithMaxNeighbours = -1;
    for(int i = 0; i < nodes; i++){
        if(colored[i] || neighboursOfCurrentSet[i] || currentSet[i]){
            continue; // Skip colored nodes
        }
        int current = getNumOfNeighboursInSet(graph, nodes, neighboursOfCurrentSet, i);
        // if the current node has more common neighbours than the max, update the max
        if(current > maxNeighbours){
            maxNeighbours = current;
            nodeWithMaxNeighbours = i;
            isMaxUnCommonSet = false;
        }
        // tie breaker heuristic
        else if(current == maxNeighbours){

            int currentUnCommon = getNumOfNeighboursNotInSet(graph, nodes, neighboursOfCurrentSet, i);
            if(!isMaxUnCommonSet){ // if max uncommon is not up to date, recompute  it
                maxUnCommon = getNumOfNeighboursNotInSet(graph, nodes, neighboursOfCurrentSet, nodeWithMaxNeighbours);
                isMaxUnCommonSet = true;
            }
            if(currentUnCommon < maxUnCommon){
                maxNeighbours = current;
                nodeWithMaxNeighbours = i;
                maxUnCommon = currentUnCommon;
            }
        }
    }

    free(neighboursOfCurrentSet);
    return nodeWithMaxNeighbours;
}

int getNumOfNeighboursInSet(int **graph, int nodes, bool *currentSet, int currentNode){
    int numOfCommonNeighbours = 0;
    for(int i = 0; i < nodes; i++){
        if(currentSet[i] && graph[currentNode][i] > 0){
            numOfCommonNeighbours++;
        }
    }
    return numOfCommonNeighbours;

}

int getNumOfNeighboursNotInSet(int **graph, int nodes, bool *currentSet, int currentNode){
    int numOfUnCommonNeighbours = 0;
    for(int i = 0; i < nodes; i++){
        if(!currentSet[i] && graph[currentNode][i] > 0){
            numOfUnCommonNeighbours++;
        }
    }
    return numOfUnCommonNeighbours;

}


int getNodeWithLargesNumberOfNeighbours(int **graph, int nodes, bool *colored){
    int maxNeighbours = 0;
    int nodeWithMaxNeighbours = -1;

    for (int i = 0; i < nodes; i++)
    {
        if (colored[i])
            continue; // Skip colored nodes

        int numOfNeighbours = getNumOfNeighbours(graph, nodes, i);
        if (numOfNeighbours > maxNeighbours)
        {
            maxNeighbours = numOfNeighbours;
            nodeWithMaxNeighbours = i;
        }
    }
    return nodeWithMaxNeighbours;
}


int getNumOfNeighbours(int **graph, int nodes, int currentNode){
    int numOfNeighbours = 0;
    for (int i = 0; i < nodes; i++)
    {
        if (graph[currentNode][i] > 0)
        {
            numOfNeighbours++;
        }
    }
    return numOfNeighbours;
}

/*
bool isNodeANeighbourToANodeInSet(int nodes, bool *currentSet, int currentNode){
    for(int i = 0; i < nodes; i++){
        if(currentSet[i] && globalGraph[currentNode][i] > 0){
            return true;
        }
    }
    return false;
}
*/



// find the next available color for the independent sets, coloring starts from 0
int findNextAvailableColor(int *solution, int nodes, bool *currentSet){
        
    int color = -1;
    for(int i = 0; i < nodes; i++){
        if(solution[i] > color){
            color = solution[i];
        }
    }
    color++; // increment the color to get the next available color
    if(color == nodes){
        return -1; // no more colors available
    }
    
    return color;

    /* THIS VERSION IS SLOWER AND GAVE THE SAME RESULT(AS FAR AS I CAN TELL)
    bool usedColors[nodes];
    memset(usedColors, 0, sizeof(bool) * nodes);

    for (int i = 0; i < nodes; i++) {
        if (solution[i] >= 0 && isNodeANeighbourToANodeInSet(nodes, currentSet, i)) {
            usedColors[solution[i]] = true;
        }
    }
    for (int c = 0; c < nodes; c++) {
        if (!usedColors[c]) return c;
    }
    return -1; // Should not reach here
    */
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


// TODO: HAVE THE FUNCTIONS BELOW THIS COMMENT IN A SEPERATE FILE

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


// Getters for the solution and order of nodes

// Get the solution array, which contains the color assignments
// -1 means no color assigned
// 0,1,2,3,... are the colors
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