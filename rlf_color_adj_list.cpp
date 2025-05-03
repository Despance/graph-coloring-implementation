#include <iostream>
#include <vector>
#include <numeric> // For std::iota
#include <algorithm> // For std::max_element, std::find
#include <chrono>    // For timing
#include <limits>    // For std::numeric_limits

// Type alias for graph representation
using Graph = std::vector<std::vector<int>>;

// Structure to hold the results
struct RLFResult {
    std::vector<int> solution; // Color assigned to each node (index = node)
    std::vector<int> orderOfNodes; // Order in which nodes were colored (might be useful for analysis)
    double executionTimeMs = 0.0; // Execution time in milliseconds
    int numberOfColorsUsed = 0;
};

// --- Function Prototypes ---

// Helper Function: Get the number of *uncolored* neighbours for a node
int getNumOfUncoloredNeighbours(int node, const Graph& adjList, const std::vector<bool>& colored);

// Helper Function: Find the uncolored node with the most uncolored neighbours
int findStartNode(int nodes, const Graph& adjList, const std::vector<bool>& colored);

// Helper Function: Get neighbours of nodes currently in set S
void getNeighboursOfSet(const Graph& adjList, const std::vector<bool>& setS, std::vector<bool>& neighboursOfS);

// Helper Function: Count neighbours of 'node' that are in 'targetSet' (typically neighboursOfS)
int getNumOfNeighboursInSet(int node, const Graph& adjList, const std::vector<bool>& targetSet);

// Helper Function: Count neighbours of 'node' that are *not* in 'targetSet' and are still candidates
int getNumOfNeighboursNotInSet(int node, const Graph& adjList, const std::vector<bool>& targetSet, const std::vector<bool>& colored, const std::vector<bool>& setS, const std::vector<bool>& neighboursOfS);

// Helper Function: Find the next best node to add to the independent set S
int findMaxIndependentNode(int nodes, const Graph& adjList, const std::vector<bool>& colored, const std::vector<bool>& setS, const std::vector<bool>& neighboursOfS);

// Helper Function: Finds the lowest non-negative integer (color) not present in the neighbours' colors
// Note: The original C code used a simpler max_color + 1 approach. This is often sufficient for RLF.
// We'll implement the simpler version first, similar to the original active C code.
int findNextAvailableColorSimple(const std::vector<int>& solution);

// Main logic for coloring one independent set
void colorMaxIndependentSet(const Graph& adjList, int nodes, std::vector<int>& solution, std::vector<int>& orderOfNodes, std::vector<bool>& colored, int startNode);

// Main RLF Algorithm Function
RLFResult rlfColor(const Graph& adjList);


// --- Function Implementations ---

int getNumOfUncoloredNeighbours(int node, const Graph& adjList, const std::vector<bool>& colored) {
    int count = 0;
    if (node < 0 || node >= adjList.size()) return 0; // Basic bounds check

    for (int neighbor : adjList[node]) {
        if (neighbor >= 0 && neighbor < colored.size() && !colored[neighbor]) {
            count++;
        }
    }
    return count;
}

int findStartNode(int nodes, const Graph& adjList, const std::vector<bool>& colored) {
    int maxNeighbours = -1; // Use -1 to ensure any uncolored node is picked initially
    int startNode = -1;

    for (int i = 0; i < nodes; ++i) {
        if (!colored[i]) {
            int numNeighbours = getNumOfUncoloredNeighbours(i, adjList, colored);
            if (numNeighbours > maxNeighbours) {
                maxNeighbours = numNeighbours;
                startNode = i;
            }
        }
    }
    return startNode; // Returns -1 if all nodes are colored
}

void getNeighboursOfSet(const Graph& adjList, const std::vector<bool>& setS, std::vector<bool>& neighboursOfS) {
    std::fill(neighboursOfS.begin(), neighboursOfS.end(), false); // Reset
    int nodes = adjList.size();
    for(int i = 0; i < nodes; ++i) {
        if(setS[i]) {
            for(int neighbor : adjList[i]) {
                if (neighbor >= 0 && neighbor < nodes) {
                    neighboursOfS[neighbor] = true;
                }
            }
        }
    }
}

int getNumOfNeighboursInSet(int node, const Graph& adjList, const std::vector<bool>& targetSet) {
    int count = 0;
     if (node < 0 || node >= adjList.size()) return 0;

    for (int neighbor : adjList[node]) {
        if (neighbor >= 0 && neighbor < targetSet.size() && targetSet[neighbor]) {
            count++;
        }
    }
    return count;
}


int getNumOfNeighboursNotInSet(int node, const Graph& adjList, const std::vector<bool>& targetSet, const std::vector<bool>& colored, const std::vector<bool>& setS, const std::vector<bool>& neighboursOfS) {
     int count = 0;
     if (node < 0 || node >= adjList.size()) return 0;

     for (int neighbor : adjList[node]) {
         // Check if the neighbour is a valid candidate node:
         // 1. Not already colored
         // 2. Not already in the current independent set (setS)
         // 3. Not a neighbour of the current independent set (neighboursOfS)
         // 4. Explicitly check if it's *not* in the targetSet (which is usually neighboursOfS itself)
         if (neighbor >= 0 && neighbor < adjList.size() &&
             !colored[neighbor] && !setS[neighbor] && !neighboursOfS[neighbor] && !targetSet[neighbor])
         {
             count++;
         }
     }
     return count;
 }


int findMaxIndependentNode(int nodes, const Graph& adjList, const std::vector<bool>& colored, const std::vector<bool>& setS, const std::vector<bool>& neighboursOfS) {
    int maxCommonNeighbours = -1;
    int minUncommonNeighbours = std::numeric_limits<int>::max();
    int bestNode = -1;

    for (int i = 0; i < nodes; ++i) {
        // Candidate nodes must be:
        // 1. Not already colored
        // 2. Not already in the current independent set S
        // 3. Not adjacent to any node currently in S (i.e., not in neighboursOfS)
        if (!colored[i] && !setS[i] && !neighboursOfS[i]) {
            int commonNeighbours = getNumOfNeighboursInSet(i, adjList, neighboursOfS); // Neighbours adjacent to S's neighbours
            
            if (commonNeighbours > maxCommonNeighbours) {
                maxCommonNeighbours = commonNeighbours;
                // Recalculate uncommon neighbours only when the primary criterion changes
                minUncommonNeighbours = getNumOfNeighboursNotInSet(i, adjList, neighboursOfS, colored, setS, neighboursOfS);
                bestNode = i;
            } else if (commonNeighbours == maxCommonNeighbours) {
                 // Tie-breaker: minimum neighbours *not* adjacent to S's neighbours
                 // (among valid candidates)
                int uncommonNeighbours = getNumOfNeighboursNotInSet(i, adjList, neighboursOfS, colored, setS, neighboursOfS);
                if (uncommonNeighbours < minUncommonNeighbours) {
                     minUncommonNeighbours = uncommonNeighbours;
                     bestNode = i;
                }
            }
        }
    }
    return bestNode; // Returns -1 if no suitable node found
}


int findNextAvailableColorSimple(const std::vector<int>& solution) {
    if (solution.empty()) return 1; // Start coloring from 1

    int max_color = 0; // Assuming colors start from 1, 0 means uncolored or initial state
    for (int color : solution) {
        if (color > max_color) {
            max_color = color;
        }
    }
    return max_color + 1; // Next available color
}


void colorMaxIndependentSet(const Graph& adjList, int nodes, std::vector<int>& solution, std::vector<int>& orderOfNodes, std::vector<bool>& colored, int startNode) {

    std::vector<bool> setS(nodes, false);
    std::vector<bool> neighboursOfS(nodes, false); // Neighbours of nodes currently in setS

    // Initial state
    setS[startNode] = true;
    orderOfNodes.push_back(startNode); // Add start node to order
    getNeighboursOfSet(adjList, setS, neighboursOfS); // Update neighbours


    while (true) {
        int nextNode = findMaxIndependentNode(nodes, adjList, colored, setS, neighboursOfS);

        if (nextNode == -1) {
            break; // No more nodes can be added to this independent set
        }

        setS[nextNode] = true;
        orderOfNodes.push_back(nextNode); // Add newly added node to order
        // Important: Update neighboursOfS immediately after adding a node to setS
        getNeighboursOfSet(adjList, setS, neighboursOfS);
    }

    // Now, color all nodes selected in setS with the next available color
    int colorToUse = findNextAvailableColorSimple(solution);
    for (int i = 0; i < nodes; ++i) {
        if (setS[i]) {
            solution[i] = colorToUse;
            colored[i] = true; // Mark as colored for future iterations
        }
    }
}


RLFResult rlfColor(const Graph& adjList) {
    auto startTime = std::chrono::high_resolution_clock::now();

    int nodes = adjList.size();
    if (nodes == 0) {
        return {{}, {}, 0.0, 0}; // Handle empty graph
    }

    RLFResult result;
    result.solution.assign(nodes, 0); // Initialize colors to 0 (or -1 if preferred)
    result.orderOfNodes.reserve(nodes); // Reserve space
    std::vector<bool> colored(nodes, false); // Track colored nodes
    int coloredCount = 0;


    while (coloredCount < nodes) {
        // STEP 1: Find the uncolored node with the largest number of *uncolored* neighbours
        int startNode = findStartNode(nodes, adjList, colored);

        if (startNode == -1) {
            break; // All remaining nodes must be isolated or graph is fully colored
        }

        // STEP 2: Build and color a maximal independent set starting from startNode
        colorMaxIndependentSet(adjList, nodes, result.solution, result.orderOfNodes, colored, startNode);

        // Update count - slightly inefficient, could track additions in colorMaxIndependentSet
        coloredCount = 0;
        for(bool c : colored) {
            if (c) coloredCount++;
        }
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    result.executionTimeMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();

    // Calculate number of colors used
     int max_c = 0;
     for(int c : result.solution) {
         if (c > max_c) max_c = c;
     }
     result.numberOfColorsUsed = max_c;


    return result;
}


// --- Example Usage ---
int main() {
    // Example Graph (Adjacency List)
    // 0: 1, 2
    // 1: 0, 2, 3
    // 2: 0, 1, 3
    // 3: 1, 2, 4
    // 4: 3
    int numNodes = 5;
    Graph adjList(numNodes);
    adjList[0].push_back(1); adjList[0].push_back(2);
    adjList[1].push_back(0); adjList[1].push_back(2); adjList[1].push_back(3);
    adjList[2].push_back(0); adjList[2].push_back(1); adjList[2].push_back(3);
    adjList[3].push_back(1); adjList[3].push_back(2); adjList[3].push_back(4);
    adjList[4].push_back(3);

    // Ensure symmetry for undirected graph representation
    for(int u=0; u<numNodes; ++u) {
        for(int v : adjList[u]) {
            // Add back edge if not present (simple check)
            bool found = false;
            if (v < adjList.size()) { // Basic bounds check
                 for(int neighbor_of_v : adjList[v]) {
                     if (neighbor_of_v == u) {
                         found = true;
                         break;
                     }
                 }
                 if (!found) {
                     adjList[v].push_back(u);
                 }
            }
        }
    }


    std::cout << "Running RLF Coloring..." << std::endl;
    RLFResult result = rlfColor(adjList);

    std::cout << "Execution Time: " << result.executionTimeMs << " ms" << std::endl;
    std::cout << "Number of Colors Used: " << result.numberOfColorsUsed << std::endl;

    std::cout << "Solution (Node: Color):" << std::endl;
    for (int i = 0; i < result.solution.size(); ++i) {
        std::cout << "  Node " << i << ": " << result.solution[i] << std::endl;
    }

    std::cout << "Order Nodes Colored:" << std::endl;
    for (int i = 0; i < result.orderOfNodes.size(); ++i) {
        std::cout << result.orderOfNodes[i] << (i == result.orderOfNodes.size() - 1 ? "" : " -> ");
    }
    std::cout << std::endl;


    return 0;
}