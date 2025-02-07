#include "Common.h"
#include "ReadCost.h"
#include "ReadGraph.h"
#include "PopulationInit.h"
#include "PopulationRandomGeneration.h"
#include "PopulationDSaturGeneration.h"
#include "CalculateFitnessForInitPopulation.h"
#include "CrossoverOperation.h"

void LocalSearch(int NodeNumber, int GraphDensity, int NumberOfPart, int GraphCount, int GraphRun, int Generation)
{
	FILE *fptr;
	char FileName[100];

	sprintf(FileName, "PBEA_WithLocalSearchLSCount_%d_%d_%d_%d.%d_%d.txt", NodeNumber, GraphDensity, NumberOfPart, GraphCount, GraphRun, Generation);

	fptr = fopen(FileName, "w");
	if (fptr == NULL)
		exit(0);

	fclose(fptr);
}

void FindBestParent(int NodeNumber, char *filename, char *outputname, double first_time_taken, double first_hit_gen, double time_taken, int NumberOfPart, int IterationSize)
{
	FILE *fptr;
	char FileName[100];

	sprintf(FileName, "%s", outputname);

	fptr = fopen(FileName, "w");
	if (fptr == NULL)
		exit(0);
	int i, j, z;
	int bestFitness = 1000000;
	int bestIndex;

	for (i = 1; i < POPULATION_SIZE; i++)
	{
		if (newPop[i][0].Fitness < bestFitness)
		{
			bestFitness = newPop[i][0].Fitness;
			bestIndex = i;
		}
	}
	fprintf(fptr, "Instance: %s\n", filename);
	fprintf(fptr, "Fitness Value: %0.2f\n", newPop[bestIndex][0].Fitness);					// Fitness value of ith parent
	fprintf(fptr, "Number of uncolored vertexes: %d\n", newPop[0][bestIndex].SpilledNodes); // Number of spilled nodes of ith parent
	fprintf(fptr, "Computation time: %.3f\n", time_taken);
	fprintf(fptr, "Iteration Size:%d\n", IterationSize);
	for (j = 1; j < NumberOfPart + 1; j++)
	{
		fprintf(fptr, "Color %d: | ", j);
		for (z = 0; z < NodeNumber; z++)
		{
			if (newPop[bestIndex][j].node[z].InPart == 1)
			{
				int node = z + 1;
				fprintf(fptr, "%d, ", node);
			}

		} // For each node of jth part of ith parent
		fprintf(fptr, "|\n");
	} // For each part
	// fprintf(fptr,"Node Number: %d\t",NodeNumber);

	fclose(fptr);
}

int main(int argc, char *argv[])
{
	if (argc != 6)
	{
		printf("enter instance_file node_number color_number iteration_size output_file");
		return 0;
	}
	int NodeNumber = atoi(argv[2]);

	time_t start, end, first_end;
	srand(time(NULL));
	int Generation;
	int GenerationCount;

	read_input(NodeNumber, argv[1]);
	int NumberOfPart = atoi(argv[3]);
	int i;
	int bestFitness = 1000000;
	int bestIndex;

	double first_time_taken = 0;
	int first_hit_gen = 0;
	PopulationInitialization(NodeNumber, NumberOfPart);
	// PopulationGeneration_1(NodeNumber, NumberOfPart, 1.0);
	DSatur_Generation(1, 1, NodeNumber, NumberOfPart, 0.4);
	DSatur_Generation(2, 41, NodeNumber, NumberOfPart, 0.4);
	DSatur_Generation(3, 81, NodeNumber, NumberOfPart, 0.2);
	CalculateSpillCostForInitPopulation(NodeNumber, NumberOfPart);
	time(&start);
	int count = 0;
	for (Generation = 1; Generation < atoi(argv[4]); Generation++)
	{
		CrossoverOperation(NumberOfPart, NodeNumber);
		count++;
		if (count == 50)
		{
			int changed = 0;
			count = 0;
			for (i = 1; i < POPULATION_SIZE; i++)
			{
				if (newPop[i][0].Fitness < bestFitness)
				{
					bestFitness = newPop[i][0].Fitness;
					bestIndex = i;
					changed = 1;
				}
			}
			if (changed)
			{
				time(&first_end);
				first_time_taken = (double)(first_end - start);
				first_hit_gen = Generation;
			}
		}

	} // Each Generation
	time(&end);
	double time_taken = (double)(end - start);

	FindBestParent(NodeNumber, argv[1], argv[5], first_time_taken, first_hit_gen, time_taken, NumberOfPart, atoi(argv[4]));

	return 1;

} // main
