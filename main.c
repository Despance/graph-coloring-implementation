// COMPILE THIS USING THE FOLLOWING COMMAND
// gcc -O3 graph_constructor.c enhanced_dsatur.c dsatur.c main.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "graph_constructor.h"
#include "dsatur.h"          // now provides dsaturSolution()
//#include "enhanced_dsatur.h" // now provides enhancedDSaturSolution()
#include "imp_color.h"       // now provides calculateImpColor(), getSolution(), getOrderOfNodes()

// It automatically uses the instance file and the enhanced solution file.
void runVerifier(const char *instFile)
{
#ifdef _WIN32
    printf("Verifier is not supported on Windows systems.\n");
#else
    char solFilename[256];
    // Extract basename from full path.
    const char *base = strrchr(instFile, '/');
    base = base ? base + 1 : instFile;
    // Enhanced solution file location.
    snprintf(solFilename, sizeof(solFilename), "solutions/sol_%s_enhanced.txt", base);

    char command[512];
    snprintf(command, sizeof(command),
             "./coloring-verifier-main/src/coloring-verifier -i %s -s %s -p1",
             instFile, solFilename);
    printf("Running verifier: %s\n", command);
    system(command);
#endif
}

// Updated function to write the coloring solution to a file.
void writeSolutionToFile(const char *baseFilename, const char *algoType, int *colors, int vertices)
{
    char solFilename[256];
    // Extract basename from full path.
    const char *base = strrchr(baseFilename, '/');
    base = base ? base + 1 : baseFilename;

    struct stat st = {0};
    if (stat("solutions", &st) == -1)
    {
        mkdir("solutions", 0755);
    }

    snprintf(solFilename, sizeof(solFilename), "solutions/sol_%s_%s.txt", base, algoType);

    FILE *solFile = fopen(solFilename, "w");
    if (!solFile)
    {
        fprintf(stderr, "Error: Could not open file %s for writing.\n", solFilename);
        return;
    }
    for (int i = 0; i < vertices; i++)
    {
        // Write color starting from 1, followed by a newline.
        fprintf(solFile, "%d\n", colors[i]);
    }
    fclose(solFile);
    printf("Solution written to %s\n", solFilename);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    int **graph = NULL;
    int vertices = 0;

    int result = constructGraphFromFile(filename, &graph, &vertices);
    if (result == 0)
    {
        printf("Graph constructed with %d vertices.\n", vertices);

        printf("Running DSatur algorithm...\n");
        int *dsaturColors = dsaturSolution(graph, vertices);
        printf("DSatur algorithm completed.\n");

        writeSolutionToFile(filename, "dsatur", dsaturColors, vertices);

        free(dsaturColors);

        int *nodeWeights = (int *)calloc(vertices, sizeof(int));
        
        printf("Running Importance Coloring algorithm...\n");
        calculateImpColor(graph, nodeWeights, vertices, 1);
        int *enhancedColors = getSolution();
        printf("Importance Coloring algorithm completed.\n");

        writeSolutionToFile(filename, "enhanced", enhancedColors, vertices);

        free(enhancedColors);

        // Automatically run the verifier for the enhanced solution.

        runVerifier(filename);

        free(nodeWeights);
        for (int i = 0; i < vertices; i++)
        {
            free(graph[i]);
        }
        free(graph);
    }
    else
    {
        printf("Failed to construct graph.\n");
    }

    return 0;
}