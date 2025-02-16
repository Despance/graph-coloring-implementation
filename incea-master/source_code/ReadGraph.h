#include "Common.h"
#define bufSize 1024
int graph[MAX_NODE][MAX_NODE];

void initialize_graph(int NodeNumber)
{
	int i, j;
	for (i = 0; i < NodeNumber; i++)
	{
		for (j = 0; j < NodeNumber; j++)
		{
			graph[i][j] = 0;
		}
	}
}
void print_graph(int NodeNumber)
{
	int i, j;
	for (i = 0; i < NodeNumber; i++)
	{
		for (j = 0; j < NodeNumber; j++)
		{
			if (graph[i][j] == 1)
			{
				printf("graph [%d][%d]\n", i, j);
			}
		}
	}
}

bool startsWith(const char *pre, const char *str)
{
	size_t lenpre = strlen(pre),
		   lenstr = strlen(str);
	return lenstr < lenpre ? false : strncmp(pre, str, lenpre) == 0;
}

void read_input(int NodeNumber, char *benchmark)
{
	FILE *fp;
	char buf[bufSize];
	char FileName[100];
	sprintf(FileName, "Multicoloring/%s", benchmark);

	if ((fp = fopen(FileName, "r")) == NULL)
	{ /* Open source file. */
		printf("fopen source-file");
		while (1)
			;
	}
	initialize_graph(NodeNumber);
	while (fgets(buf, sizeof(buf), fp) != NULL)
	{
		buf[strlen(buf) - 1] = '\0'; // eat the newline fgets() stores
		if (startsWith("e", buf))
		{
			int init_size = strlen(buf);
			char delim[] = " ";
			char *ptr = strtok(buf, delim);
			int count = 0;
			int firstnode, secondnode;
			while (ptr != NULL)
			{
				if (count == 1)
					firstnode = atoi(ptr);
				if (count == 2)
					secondnode = atoi(ptr);
				ptr = strtok(NULL, delim);
				count++;
			}
			if (firstnode != secondnode)
			{
				graph[firstnode - 1][secondnode - 1] = 1;
				graph[secondnode - 1][firstnode - 1] = 1;
			}
		}

		if (startsWith("n", buf))
		{
			int init_size = strlen(buf);
			char delim[] = "    ";
			char *ptr = strtok(buf, delim);
			int count = 0;
			int nodeid, nodeweight;
			while (ptr != NULL)
			{
				if (count == 1)
				{
					nodeid = atoi(ptr);
				}

				if (count == 2)
				{
					nodeweight = atoi(ptr);
				}

				ptr = strtok(NULL, delim);
				count++;
			}
			CostArray[nodeid - 1] = nodeweight;
		}
	}
	fclose(fp);

	// print_graph(NodeNumber);
	// PrintCost(NodeNumber);
}
