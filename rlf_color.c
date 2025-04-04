#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>



int *solution; // Solution array

int *orderOfNodes; // Order of nodes array

time_t exec_time_imp;

double totalTime_imp = 0.0; // Total execution time


void rlfColor(int **graph, int nodes){

    // Initialize solution and orderOfNodes arrays
    solution = (int *)calloc(nodes, sizeof(int)); // Solution array
    orderOfNodes = (int *)malloc(nodes * sizeof(int));
    bool *colored = (bool *)calloc(nodes, sizeof(bool));

    standartRLFColor(graph, nodes, solution, orderOfNodes, colored);
    
    // Free allocated memory
    free(colored);

}


void standartRLFColor(int **graph, int nodes, int *solution, int *orderOfNodes, int *colored){


    // if node with largest num of neighbours can not be found(method returns -1), stop coloring
    int currentNode = 0;
    while(true){

        // STEP 1: Find the node with the largest number of neighbours as a starting point
        currentNode = getNodeWithLargesNumberOfNeighbours(graph, nodes, colored);
        if(currentNode == -1){
            break; // No more uncolored nodes
        }
        // STEP 2: find the maximum independent set in the graph and color the nodes
        // remove these colored nodes from the graph
    }

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
// call findMaxIndependentNode, add it to the set S
// add the node to order of Nodes
// do this until no more nodes can be added to the set S
// remove the nodes in set S from the graph
// color the nodes in set S
// remove the nodes in set S from the graph(graph should be temporary)
}



int findMaxIndependentNode(int **graph, int nodes, bool *colored, bool *currentSet){

    // TODO: instead of allocating memory for this array every time, we can just reuse it
    // memset it to false
    bool *neihbourOfCurrentSet = (bool *)calloc(nodes, sizeof(bool)); 
    for(int i = 0; i < nodes; i++){
        if(currentSet[i]){
            for(int j = 0; j < nodes; j++){
                if(graph[i][j] > 0){
                    neihbourOfCurrentSet[j] = true;
                }
            }
        }
    }

    int maxNeighbours = 0;
    int maxUnCommon = 0x7FFFFFFF; // init to largest value
    bool isMaxUnCommonSet = false;
    int nodeWithMaxNeighbours = -1;
    for(int i = 0; i < nodes; i++){
        
        if(colored[i] || neihbourOfCurrentSet[i] || currentSet[i]){
            continue; // Skip colored nodes
        }

        int current = getNumOfNeighboursInSet(graph, nodes, currentSet, i);
        // if the current node has more common neighbours than the max, update the max
        if(current > maxNeighbours){
            maxNeighbours = current;
            nodeWithMaxNeighbours = i;
            isMaxUnCommonSet = false;
        }
        // tie breaker heuristic
        else if(current == maxNeighbours){

            int currentUnCommon = getNumOfNeighboursNotInSet(graph, nodes, currentSet, i);
            if(!isMaxUnCommonSet){ // if max uncommon is not up to date, recompute  it
                maxUnCommon = getNumOfNeighboursNotInSet(graph, nodes, currentSet, nodeWithMaxNeighbours);
            }
            if(currentUnCommon < maxUnCommon){
                maxNeighbours = current;
                nodeWithMaxNeighbours = i;
                maxUnCommon = currentUnCommon;
                isMaxUnCommonSet = true;
            }
        }
    }

    free(neihbourOfCurrentSet);
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