#include "Common.h"

int FindMaxConlictNodeInPart(int ParentIndex, int NodeNumber, int Part, int NodeSelected[], int ConflictNumber[])
{
	int z, MaxConflictNode = -1, MaxConflict = 0;
	for (z = 0; z < NodeNumber; z++)
	{
		if (newPop[ParentIndex][Part].node[z].InPart == 1 && NodeSelected[z] == 0 && ConflictNumber[z] != 0)
		{
			if (ConflictNumber[z] > MaxConflict)
			{
				MaxConflictNode = z;
				MaxConflict = ConflictNumber[z];
			}
			else if (ConflictNumber[z] == MaxConflict && CostArray[z] < CostArray[MaxConflictNode])
			{
				MaxConflictNode = z;
				MaxConflict = ConflictNumber[z];
			}
			else
				continue;
		}
	}
	if (MaxConflictNode != -1)
	{
		NodeSelected[MaxConflictNode] = 1;
		// reducing conflicts
		for (z = 0; z < NodeNumber; z++)
		{
			if (newPop[ParentIndex][Part].node[z].InPart == 1 && graph[z][MaxConflictNode] == 1)
				ConflictNumber[z]--;
		}
	}

	return MaxConflictNode;
}

void TransferConflict(int PopulationIndex, int NodeNumber, int NumberOfPart, int ConflictNumber[])
{
	int i, j;
	for (i = 1; i < NumberOfPart + 1; i++)
	{
		for (j = 0; j < NodeNumber; j++)
		{
			if (newPop[PopulationIndex][i].node[j].InPart == 1)
				ConflictNumber[j] = newPop[PopulationIndex][i].node[j].ConflictNumber;
		} // Each Node
	} // Each Part
}
void CalculateSpillCostForInitPopulation(int NodeNumber, int NumberOfPart)
{
	int i, j;
	int NodeSelected[NodeNumber];
	int ConflictNumber[NodeNumber];
	for (i = 1; i < POPULATION_SIZE; i++)
	{
		InitNodeSelected(NodeNumber, NodeSelected);
		TransferConflict(i, NodeNumber, NumberOfPart, ConflictNumber);
		for (j = 1; j < NumberOfPart + 1; j++)
		{
			while (1)
			{
				int node = FindMaxConlictNodeInPart(i, NodeNumber, j, NodeSelected, ConflictNumber);
				if (node == -1)
					break;
				newPop[i][j].SpilledNodes++;
				newPop[i][j].Fitness += CostArray[node];
				newPop[i][0].Fitness += CostArray[node];
				newPop[0][i].SpilledNodes++;
			}
		} // Each part
	} // Each Parent
}