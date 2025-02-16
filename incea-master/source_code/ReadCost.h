#include "Common.h"

int CostArray[MAX_NODE];

void PrintCost(int NodeNumber)
{
	int i;
	for (i = 0; i < NodeNumber; i++)
		printf("%d\n", CostArray[i]);
}
void ReadCost(int NodeNumber)
{
	char FileName[100];
	sprintf(FileName, "GEO_%d_cost.txt", NodeNumber);
	FILE *fptr;
	fptr = fopen(FileName, "r");

	if (fptr == NULL)
	{
		printf("Error in reading file cost in reading\n");
		while (1)
			;
	}

	int CostCount = 0;
	while (CostCount < NodeNumber)
	{
		fscanf(fptr, "%d", &CostArray[CostCount]);
		CostCount++;
	}

	fclose(fptr);
}