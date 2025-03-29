// COMPILE THIS USING THE FOLLOWING COMMAND
// gcc -O3 graph_constructor.c enhanced_dsatur.c dsatur.c main.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "graph_constructor.h"
#include "dsatur.h" // now provides dsaturSolution()
// #include "enhanced_dsatur.h" // now provides enhancedDSaturSolution()
#include "imp_color.h" // now provides calculateImpColor(), getSolution(), getOrderOfNodes()
#include "other_algorithms/rlf.h" // now provides rlfGraphColoring()
#include "other_algorithms/wp.h" // now provides wpGraphColoring()
#include "other_algorithms/ldo.h" // now provides ldoGraphColoring()

// It automatically uses the instance file and the enhanced solution file.
void runVerifier(const char *instFile)
{
#ifdef _WIN32
    printf("Verifier is not supported on Windows systems.\n");
#else
    char solFilename[256];
    char sol2Filename[256];
    // Extract basename from full path.
    const char *base = strrchr(instFile, '/');
    base = base ? base + 1 : instFile;
    // Enhanced solution file location.
    snprintf(sol2Filename, sizeof(sol2Filename), "solutions/%s/sol_%s_dsatur.txt", base, base);
    snprintf(solFilename, sizeof(solFilename), "solutions/%s/sol_%s_importance.txt", base, base);

    char command[512];

    snprintf(command, sizeof(command),
             "./coloring-verifier-main/src/coloring-verifier -i %s -s %s -p1",
             instFile, sol2Filename);
    printf("Running verifier: %s\n", command);
    system(command);

    snprintf(command, sizeof(command),
             "./coloring-verifier-main/src/coloring-verifier -i %s -s %s -p1",
             instFile, solFilename);
    printf("Running verifier: %s\n", command);
    system(command);

#endif
}

// Updated function to write the coloring solution to a file.
void writeSolutionToFile(const char *baseFilename, const char *algoType, int *colors, int vertices, int *order)
{
    char solFilename[256];
    // Extract basename from full path.
    const char *base = strrchr(baseFilename, '/');
    base = base ? base + 1 : baseFilename;

    struct stat st = {0};
    // Create "solutions" directory if it doesn't exist.
    if (stat("solutions", &st) == -1)
    {
        mkdir("solutions", 0755);
    }
    // Create a subdirectory inside solutions for this instance.
    char solDir[256];
    snprintf(solDir, sizeof(solDir), "solutions/%s", base);
    if (stat(solDir, &st) == -1)
    {
        mkdir(solDir, 0755);
    }

    // Build the solution file path inside the subdirectory.
    snprintf(solFilename, sizeof(solFilename), "%s/sol_%s_%s.txt", solDir, base, algoType);

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

    if (order == NULL)
        return;

    char orderFilename[256];
    //
    snprintf(orderFilename, sizeof(orderFilename), "%s/order_%s_%s.txt", solDir, base, algoType);
    FILE *orderFile = fopen(orderFilename, "w");
    if (!orderFile)
    {
        fprintf(stderr, "Error: Could not open file %s for writing.\n", orderFilename);
        return;
    }
    for (int i = 0; i < vertices; i++)
    {
        // Write color starting from 1, followed by a newline.
        fprintf(orderFile, "%d\n", order[i]);
    }

    fclose(orderFile);
    printf("Order written to %s\n", orderFilename);
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
        dsatur(graph, vertices);
        int *dsaturColors = getDsaturSolution();
        printf("DSatur algorithm completed.\n");
        writeSolutionToFile(filename, "dsatur", dsaturColors, vertices, getDsaturOrderOfNodes());

        free(dsaturColors);

        int *nodeWeights = (int *)calloc(vertices, sizeof(int));

        printf("Running Importance Coloring algorithm...\n");
        calculateImpColor(graph, nodeWeights, vertices, 1);
        int *enhancedColors = getSolution();
        printf("Importance Coloring algorithm completed.\n");

        writeSolutionToFile(filename, "importance", enhancedColors, vertices, getOrderOfNodes());

        free(enhancedColors);

        // Run the RLF algorithm.
        printf("Running RLF algorithm...\n");
        rlfGraphColoring(graph, vertices);
        printf("RLF algorithm completed.\n");
        int *rlfColours = getRlfSolution();

        writeSolutionToFile(filename, "rlf", rlfColours, vertices, getRlfSolutionOrder());
        free(rlfColours);

        // Run the Welsh-Powell algorithm.
        printf("Running Welsh-Powell algorithm...\n");
        wpGraphColoring(graph, vertices);
        printf("Welsh-Powell algorithm completed.\n");
        int *wpColours = getWpSolution();

        writeSolutionToFile(filename, "wp", wpColours, vertices, getWpOrder());
        free(wpColours);

        // Run the ldo algorithm.
        printf("Running LDO algorithm...\n");
        ldoGraphColoring(graph, vertices);
        printf("LDO algorithm completed.\n");
        int *ldoColours = getLdoSolution();

        writeSolutionToFile(filename, "ldo", ldoColours, vertices, getLdoOrder());
        free(ldoColours);

        // Automatically run the verifier for the enhanced solution.

        runVerifier(filename);

        printf("DSatur-Time: %lf \n", totalTime_DSatur);
        printf("Importance-Time: %lf", totalTime_imp);
        //printf("RLF-Time: %lf", totalTime_rlf);
        //printf("Welsh-Powell-Time: %lf", totalTime_wp);

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