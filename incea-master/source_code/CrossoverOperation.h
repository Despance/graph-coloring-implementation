#include "Common.h"

int Pool[MAX_NODE]; // Global
int SelectedNodesForOffspring[MAX_NODE];
int SelectionOfParents()
{
	return rand() % 100 + 1;
}

int ControlOfUsedPart(int SelectionOfParts[], int index)
{
	if (index == 1)
		return 0;
	int i;
	for (i = 1; i < index; i++)
	{
		if (SelectionOfParts[i] == SelectionOfParts[index])
			return 1;
	}
	return 0;
}

void CalculateConflictForOffspring(int PartIndex, int NodeNumber, int Node)
{
	int z; // each node
	for (z = 0; z < NodeNumber; z++)
	{
		if (graph[z][Node] == 1 && OffspringPart[PartIndex].node[z].InPart == 1)
		{
			OffspringPart[PartIndex].node[z].ConflictNumber++;
			OffspringPart[PartIndex].node[Node].ConflictNumber++;
		}
	}
}

int FindMaxCostNodeForPool(int NodeNumber)
{
	int i; // EachNode
	int MaxCostNode = -1;
	int MaxCost = 0;
	for (i = 0; i < NodeNumber; i++)
	{
		if (Pool[i] == 1 && MaxCost < CostArray[i] && SelectedNodesForOffspring[i] == 0)
		{
			MaxCostNode = i;
			MaxCost = CostArray[i];
		} // if
	} // for
	return MaxCostNode;
}

int FindMaxConflictNodeForOffspring(int PartIndex, int NodeNumber)
{
	int z; // Each node
	int MaxConflict = 0;
	int maxConflictNode = -1;
	for (z = 0; z < NodeNumber; z++)
	{
		if (OffspringPart[PartIndex].node[z].InPart == 1 && OffspringPart[PartIndex].node[z].ConflictNumber > MaxConflict)
		{
			MaxConflict = OffspringPart[PartIndex].node[z].ConflictNumber;
			maxConflictNode = z;
		}
		else if (OffspringPart[PartIndex].node[z].InPart == 1 && OffspringPart[PartIndex].node[z].ConflictNumber == MaxConflict && MaxConflict != 0 && CostArray[z] < CostArray[maxConflictNode])
		{
			MaxConflict = OffspringPart[PartIndex].node[z].ConflictNumber;
			maxConflictNode = z;
		}
		else
			continue;
	}
	if (maxConflictNode != -1)
	{
		OffspringPart[PartIndex].node[maxConflictNode].InPart = 0;
		OffspringPart[PartIndex].node[maxConflictNode].ConflictNumber = 0;
		for (z = 0; z < NodeNumber; z++)
		{
			if (OffspringPart[PartIndex].node[z].InPart == 1 && graph[maxConflictNode][z] == 1)
				OffspringPart[PartIndex].node[z].ConflictNumber--;
		}
	}
	return maxConflictNode;
}

void SearchBack(int NumberOfCombination, int NodeNumber)
{
	int Conflict[NodeNumber];
	int i;
	for (i = 0; i < NodeNumber; i++)
		Conflict[i] = 0;
	// printf("\nIn search back\n");
	// sleep(2);
	if (NumberOfCombination > 1)
	{ // Suanda bulundugu partı parametre olarak al
		int Part = 1;
		while (Part != NumberOfCombination)
		{
			int NodeInPool, NodeInOffspring;
			for (NodeInPool = 0; NodeInPool < NodeNumber; NodeInPool++)
			{
				for (NodeInOffspring = 0; NodeInOffspring < NodeNumber; NodeInOffspring++)
				{
					if (Pool[NodeInPool] == 1 && OffspringPart[Part].node[NodeInOffspring].InPart == 1 && graph[NodeInOffspring][NodeInPool] == 1)
					{
						Conflict[NodeInPool] = 1;
					} // if
				} // NodeInOffspring

				if (Conflict[NodeInPool] == 0 && Pool[NodeInPool] == 1)
				{
					//	printf("\n%d is inserted at %d part\n",NodeInPool,Part );
					//	sleep(3);
					Pool[NodeInPool] = 0;
					SelectedNodesForOffspring[NodeInPool] = 1;
					OffspringPart[Part].node[NodeInPool].InPart = 1;
				} // if

				else
				{
					Conflict[NodeInPool] = 0;
				} // else

			} // NodeInPool

			Part++;
		} // while

	} // NumOfCom > 0
}

void initUpdatedParent(int ParentIndex, int NodeNumber, int NumberOfPart)
{
	int j;									 // Each Part
	newPop[ParentIndex][0].Fitness = 0;		 // Fitness value of ith parent
	newPop[0][ParentIndex].SpilledNodes = 0; // Number of spilled nodes of ith parent
	for (j = 1; j < NumberOfPart + 1; j++)
	{
		int z; // Each Node
		for (z = 0; z < NodeNumber; z++)
		{
			newPop[ParentIndex][j].node[z].InPart = 0; // means that zth node does not participate in jth part of ith parent
			newPop[ParentIndex][j].node[z].ConflictNumber = 0;
			newPop[ParentIndex][j].SpilledNodes = 0;
			newPop[ParentIndex][j].Fitness = 0;

		} // For each node of jth part of ith parent
	} // For each part
}
void UpdatePopulation(int ParentIndex, int NodeNumber, int NumberOfPart, double FitnessOfOffspring, int SpilledNodeOfOffsring)
{
	int i; // Each Part
	int j; // Each node
	initUpdatedParent(ParentIndex, NodeNumber, NumberOfPart);
	for (j = 1; j < NumberOfPart + 1; j++)
	{
		for (i = 0; i < NodeNumber; i++)
		{
			if (OffspringPart[j].node[i].InPart == 1)
			{
				newPop[ParentIndex][j].node[i].InPart = 1;
				newPop[ParentIndex][j].node[i].ConflictNumber = OffspringPart[j].node[i].ConflictNumber;
			}
		} // Each node
	} // Each Part
	newPop[ParentIndex][0].Fitness = FitnessOfOffspring;
	newPop[0][ParentIndex].SpilledNodes = SpilledNodeOfOffsring;
}

void CrossoverOperation(int NumberOfPart, int NodeNumber)
{
	int SelectedParentIndex1, SelectedParentIndex2;
	SelectedParentIndex1 = SelectionOfParents();
	while (1)
	{
		SelectedParentIndex2 = SelectionOfParents();
		if (SelectedParentIndex1 != SelectedParentIndex2)
			break;
	}
	// printf("\n\nSelected Parent Indexs are %d________%d\n\n", SelectedParentIndex1,SelectedParentIndex2);
	// sleep(3);
	/****************************************************************************/
	int SelectionOfParts1[NumberOfPart + 1], SelectionOfParts2[NumberOfPart + 1];
	int NumberOfCombination;
	// SelectionOfParts[NumberOfCombination] number of part for ith combination
	for (NumberOfCombination = 1; NumberOfCombination < NumberOfPart + 1; NumberOfCombination++)
	{
		SelectionOfParts1[NumberOfCombination] = rand() % NumberOfPart + 1;
		while (ControlOfUsedPart(SelectionOfParts1, NumberOfCombination) == 1)
			SelectionOfParts1[NumberOfCombination] = rand() % NumberOfPart + 1;

		SelectionOfParts2[NumberOfCombination] = rand() % NumberOfPart + 1;
		while (ControlOfUsedPart(SelectionOfParts2, NumberOfCombination) == 1)
			SelectionOfParts2[NumberOfCombination] = rand() % NumberOfPart + 1;
	}
	for (NumberOfCombination = 1; NumberOfCombination < NumberOfPart + 1; NumberOfCombination++)
	{
		// printf("\n\nFor %dth Combination Part Index %d______%d\n\n",NumberOfCombination, SelectionOfParts1[NumberOfCombination], SelectionOfParts2[NumberOfCombination] );
		//	sleep(3);
	}
	/*******************************************************************************/
	TempPartOfOffspringInitialization(NodeNumber, NumberOfPart);

	int i; // Each node
	int NodeNum;
	for (i = 0; i < NodeNumber; i++)
		Pool[i] = 0;

	for (i = 0; i < NodeNumber; i++)
		SelectedNodesForOffspring[i] = 0;

	for (NumberOfCombination = 1; NumberOfCombination < NumberOfPart + 1; NumberOfCombination++)
	{
		//	printf("\nNEW COMBIN\n");
		//	sleep(3);
		for (NodeNum = 0; NodeNum < NodeNumber; NodeNum++)
		{
			if ((Pool[NodeNum] == 1 || newPop[SelectedParentIndex1][SelectionOfParts1[NumberOfCombination]].node[NodeNum].InPart == 1 || newPop[SelectedParentIndex2][SelectionOfParts2[NumberOfCombination]].node[NodeNum].InPart == 1) && *(SelectedNodesForOffspring + NodeNum) == 0)
			{
				OffspringPart[NumberOfCombination].node[NodeNum].InPart = 1;
				SelectedNodesForOffspring[NodeNum] = 1;
				Pool[NodeNum] = 0;
				//			printf("%d node is inserted to the %d part \n",NodeNum,NumberOfCombination );
				//			sleep(2);
				CalculateConflictForOffspring(NumberOfCombination, NodeNumber, NodeNum);
			}
		} // Each node
		//	printf("\n\nPool   :  \n");
		while (1)
		{
			int maxConflictNode = FindMaxConflictNodeForOffspring(NumberOfCombination, NodeNumber);
			if (maxConflictNode == -1)
				break;
			//		printf("%d   ",maxConflictNode );
			SelectedNodesForOffspring[maxConflictNode] = 0;
			Pool[maxConflictNode] = 1;
		} // while
		SearchBack(NumberOfCombination, NodeNumber);

	} // Each Combination

	/***********************************************************************************/
	/**********************LOCAL SEARCH******************************/

	// printf("\nIN LOCAL SEARCH\n");
	// sleep(3);
	for (i = 0; i < NodeNumber; i++) // bir kez aranmış ama bir yere konamamış nodelara tekrar arama yapmamak için kontrol
		SelectedNodesForOffspring[i] = 0;
	while (1)
	{
		int MaxCostNode = FindMaxCostNodeForPool(NodeNumber);

		if (MaxCostNode == -1)
			break;
		//	printf("\n%d node is selected from pool for LS\n",MaxCostNode );
		//	sleep(3);
		int EachPart;
		int minCostForSwap = CostArray[MaxCostNode];
		int minCostPartIndex = -1;
		int SumOfCost[NumberOfPart + 1];
		for (i = 1; i < NumberOfCombination + 1; i++)
		{
			SumOfCost[i] = 0;
		}

		for (EachPart = 1; EachPart < NumberOfPart + 1; EachPart++)
		{
			for (NodeNum = 0; NodeNum < NodeNumber; NodeNum++)
			{
				if (OffspringPart[EachPart].node[NodeNum].InPart == 1 && graph[NodeNum][MaxCostNode] == 1)
				{
					SumOfCost[EachPart] += CostArray[NodeNum];
				} // if
			} // NodeNum

			if (SumOfCost[EachPart] < minCostForSwap)
			{
				minCostForSwap = SumOfCost[EachPart];
				minCostPartIndex = EachPart;
			}

		} // EachPart

		if (minCostPartIndex != -1)
		{
			Pool[MaxCostNode] = 0;
			OffspringPart[minCostPartIndex].node[MaxCostNode].InPart = 1;
			//		printf("\n%d node is inserted the %d part of the offspring\n", MaxCostNode, minCostPartIndex);
			//		sleep(3);
			for (NodeNum = 0; NodeNum < NodeNumber; NodeNum++)
			{
				if (OffspringPart[minCostPartIndex].node[NodeNum].InPart == 1 && graph[NodeNum][MaxCostNode] == 1)
				{
					OffspringPart[minCostPartIndex].node[NodeNum].InPart = 0;
					OffspringPart[minCostPartIndex].node[NodeNum].ConflictNumber = 0;
					Pool[NodeNum] = 1;
					//				printf("\n%d is inserted to the pool\n",NodeNum );
				} // if

			} // for
			//		sleep(3);
			CalculateConflictForOffspring(minCostPartIndex, NodeNumber, MaxCostNode);
		} // minCostPartIndex
		else
			SelectedNodesForOffspring[MaxCostNode] = 1;

	} // while
	/**********************LOCAL SEARCH******************************/
	/***********************************************************************************/
	// CALCULATE FITNESS WITHOUT LOCAL SEARCH
	double FitnessOfOffspring = 0;
	int SpilledNodeOfOffsring = 0;
	// printf("Spilled Nodes:	");
	for (i = 0; i < NodeNumber; i++)
	{
		if (Pool[i] == 1)
		{
			//		printf("%d   ",i );
			int PartIndex = rand() % NumberOfPart + 1;
			Pool[i] = 0;
			OffspringPart[PartIndex].node[i].InPart = 1;
			CalculateConflictForOffspring(PartIndex, NodeNumber, i);
			FitnessOfOffspring += CostArray[i];
			SpilledNodeOfOffsring++;
		}
	}
	// printf("\n\nFitnessOfOffspring %0.2f\nSelectedNodesForOffspring %d\n",FitnessOfOffspring,SpilledNodeOfOffsring );
	// printf("Fitness of  1st parent %0.2f\n",newPop[SelectedParentIndex1][0].Fitness);
	// printf("Fitness of  2nd parent %0.2f\n",newPop[SelectedParentIndex2][0].Fitness);
	if (FitnessOfOffspring < newPop[SelectedParentIndex1][0].Fitness)
	{
		if (FitnessOfOffspring < newPop[SelectedParentIndex2][0].Fitness && newPop[SelectedParentIndex2][0].Fitness > newPop[SelectedParentIndex1][0].Fitness)
		{
			UpdatePopulation(SelectedParentIndex2, NodeNumber, NumberOfPart, FitnessOfOffspring, SpilledNodeOfOffsring);
			//			printf("Updated with 2nd parent  which is %d\n",SelectedParentIndex2);
		}
		else
		{
			UpdatePopulation(SelectedParentIndex1, NodeNumber, NumberOfPart, FitnessOfOffspring, SpilledNodeOfOffsring);
			//			printf("Updated with 1st parent which is %d\n",SelectedParentIndex1);
		}
	}
	else
	{
		if (FitnessOfOffspring < newPop[SelectedParentIndex2][0].Fitness)
		{
			UpdatePopulation(SelectedParentIndex2, NodeNumber, NumberOfPart, FitnessOfOffspring, SpilledNodeOfOffsring);
			//			printf("Updated with 2nd parent which is %d\n",SelectedParentIndex2);
		}
	}
}
