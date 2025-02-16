#include "Common.h"

typedef struct nod
{
	int InPart;
	int ConflictNumber;
} n;

typedef struct population
{
	n node[MAX_NODE]; // keeps nodes
	double Fitness;
	int SpilledNodes;
} pop;

/***************************/
// For crossover and local search

typedef struct Offspring
{
	n node[MAX_NODE];
} Offs;
Offs OffspringPart[MAX_PART];

/****************************/

pop newPop[POPULATION_SIZE][MAX_PART]; // newPop[0][0] is not used. newPop[0][N] is used for the spilled nodes of Nth Parent. newPop[N][0] is used for fitness of Nth Parent.
void PrintPop(int NodeNumber, int NumberOfPart)
{
	int i, j;
	for (i = 1; i < POPULATION_SIZE; i++)
	{
		printf("Fitness Value of %dth parent: %0.2f\n", i, newPop[i][0].Fitness);			 // Fitness value of ith parent
		printf("Number of spilled nodes of %dth parent %d\n", i, newPop[0][i].SpilledNodes); // Number of spilled nodes of ith parent
		for (j = 1; j < NumberOfPart + 1; j++)
		{
			printf("FOR %dth part of the %dth parent:\n", j, i);
			int z;
			printf("SpilledNodes of the part : %d\n", newPop[i][j].SpilledNodes);
			printf("Fitness of the part : %0.2f\n", newPop[i][j].Fitness);
			for (z = 0; z < NodeNumber; z++)
			{
				if (newPop[i][j].node[z].InPart == 1)
				{
					printf("%d : %d \n", z, newPop[i][j].node[z].ConflictNumber);
				}

			} // For each node of jth part of ith parent
		} // For each part
	} // For each parent
} // end of function

void PopulationInitialization(int NodeNumber, int NumberOfPart)
{
	int i, j;
	for (i = 1; i < POPULATION_SIZE; i++)
	{
		newPop[i][0].Fitness = 0;	   // Fitness value of ith parent
		newPop[0][i].SpilledNodes = 0; // Number of spilled nodes of ith parent
		for (j = 1; j < NumberOfPart + 1; j++)
		{
			int z;
			for (z = 0; z < NodeNumber; z++)
			{
				newPop[i][j].node[z].InPart = 0; // means that zth node does not participate in jth part of ith parent
				newPop[i][j].node[z].ConflictNumber = 0;
				newPop[i][j].SpilledNodes = 0;
				newPop[i][j].Fitness = 0;

			} // For each node of jth part of ith parent
		} // For each part
	} // For each parent
} // PopulationInitialization

void TempPartOfOffspringInitialization(int NodeNumber, int NumberOfPart)
{
	int i, j;
	for (i = 1; i < NumberOfPart + 1; i++)
	{
		for (j = 0; j < NodeNumber; j++)
		{
			OffspringPart[i].node[j].InPart = 0;
			OffspringPart[i].node[j].ConflictNumber = 0;
		}
	}
}