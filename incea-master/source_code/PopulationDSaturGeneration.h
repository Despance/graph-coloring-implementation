#include "Common.h"

int FindMaxConflictInGraph(int Node, int NodeNumber)
{
	int Node2, ConflictNumber = 0;
	for (Node2 = 0; Node2 < NodeNumber; Node2++)
	{
		if (graph[Node][Node2] == 1)
			ConflictNumber++;
	}
	return ConflictNumber;
}
int FindMaxConflictCost(int NodeNumber, int NodeSelected[])
{
	int maxNode = -1;
	int MaxCostConflict = -1;
	int NodeCount;
	for (NodeCount = 0; NodeCount < NodeNumber; NodeCount++)
	{
		int NodeConflict = FindMaxConflictInGraph(NodeCount, NodeNumber);
		if (NodeConflict * CostArray[NodeCount] > MaxCostConflict && NodeSelected[NodeCount] == 0)
		{
			if (maxNode != -1)
				NodeSelected[maxNode] = 0;
			MaxCostConflict = NodeConflict * CostArray[NodeCount];
			maxNode = NodeCount;
			NodeSelected[maxNode] = 1;
		}
	}
	return maxNode;
}

int FindMaxConflictConflictCost(int NodeNumber, int NodeSelected[])
{
	int maxNode = -1;
	int MaxCostConflict = -1;
	int NodeCount;
	for (NodeCount = 0; NodeCount < NodeNumber; NodeCount++)
	{
		int NodeConflict = FindMaxConflictInGraph(NodeCount, NodeNumber);
		if (NodeConflict * NodeConflict * CostArray[NodeCount] > MaxCostConflict && NodeSelected[NodeCount] == 0)
		{
			if (maxNode != -1)
				NodeSelected[maxNode] = 0;
			MaxCostConflict = NodeConflict * NodeConflict * CostArray[NodeCount];
			maxNode = NodeCount;
			NodeSelected[maxNode] = 1;
		}
	}
	return maxNode;
}

int FindMaxCost(int NodeNumber, int NodeSelected[])
{
	int maxNode = -1;
	int MaxCostConflict = -1;
	int NodeCount;
	for (NodeCount = 0; NodeCount < NodeNumber; NodeCount++)
	{
		if (CostArray[NodeCount] > MaxCostConflict && NodeSelected[NodeCount] == 0)
		{
			if (maxNode != -1)
				NodeSelected[maxNode] = 0;
			MaxCostConflict = CostArray[NodeCount];
			maxNode = NodeCount;
			NodeSelected[maxNode] = 1;
		}
	}
	return maxNode;
}

void InitNodeSelected(int NodeNumber, int NodeSelected[])
{
	int i;
	for (i = 0; i < NodeNumber; i++)
		NodeSelected[i] = 0;
}

// Type of DSatur for 1 : conflict*cost sorting / 2: conlict^2*cost sorting / 3: cost sorting
void DSatur_Generation(int TypeofDSatur, int StartPopIndex, int NodeNumber, int NumberOfPart, double rate)
{
	int PopulationSize = rate * (POPULATION_SIZE - 1) + StartPopIndex;
	int node, PartIndex;
	int NodeSelected[NodeNumber];
	for (; StartPopIndex < PopulationSize; StartPopIndex++)
	{
		InitNodeSelected(NodeNumber, NodeSelected);
		while (1)
		{
			if (TypeofDSatur == 1)
				node = FindMaxConflictCost(NodeNumber, NodeSelected);
			else if (TypeofDSatur == 2)
				node = FindMaxConflictConflictCost(NodeNumber, NodeSelected);
			else
				node = FindMaxCost(NodeNumber, NodeSelected);

			if (node == -1)
				break;

			PartIndex = rand() % NumberOfPart + 1;
			newPop[StartPopIndex][PartIndex].node[node].InPart = 1;
			CalculateConflictForRandomGeneration(NodeNumber, StartPopIndex, PartIndex, node);
		}
	}
}
