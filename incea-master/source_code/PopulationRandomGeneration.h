#include "Common.h"

void CalculateConflictForRandomGeneration(int NodeNumber, int PopulationIndex, int PartIndex, int node)
{
	int z;
	for (z = 0; z < NodeNumber; z++)
	{
		if (newPop[PopulationIndex][PartIndex].node[z].InPart == 1 && graph[node][z] == 1)
		{
			newPop[PopulationIndex][PartIndex].node[z].ConflictNumber++;
			newPop[PopulationIndex][PartIndex].node[node].ConflictNumber++;
		}
	}
}
void PopulationGeneration_1(int NodeNumber, int NumberOfPart, double rate)
{
	int PopulationSize = rate * (POPULATION_SIZE - 1);
	int i, z, PartIndex;
	for (i = 1; i < PopulationSize + 1; i++)
	{
		for (z = 0; z < NodeNumber; z++)
		{

			PartIndex = rand() % NumberOfPart + 1;
			newPop[i][PartIndex].node[z].InPart = 1;
			CalculateConflictForRandomGeneration(NodeNumber, i, PartIndex, z);

		} // Each Node
	} // Each Parent
}
