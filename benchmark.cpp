#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#define MAX_FILENAME_LENGTH 256
#define MAX_COMMAND_LENGTH 512

void run_benchmark(const char *filename)
{
    char command[MAX_COMMAND_LENGTH];
    snprintf(command, sizeof(command), "./a.out %s", filename);

    FILE *fp = popen(command, "r");
    if (fp == NULL)
    {
        perror("popen");
        exit(EXIT_FAILURE);
    }

    char output[1024];
    int dsatur = -1;
    int enhanced_dsatur = -1;

    while (fgets(output, sizeof(output), fp) != NULL)
    {
        if (
            sscanf(output, "DSatur: %d", &dsatur) == 1 || sscanf(output, "Enhanced DSatur: %d", &enhanced_dsatur) == 1)
            ;
    }

    pclose(fp);

    printf("%-40s %10d %20d\n", filename, dsatur, enhanced_dsatur);
}

int main()
{
    DIR *d;
    struct dirent *dir;
    d = opendir("./datasets");

    if (d)
    {
        printf("%-40s %10s %20s\n", "Filename", "DSatur", "Enhanced DSatur");
        while ((dir = readdir(d)) != NULL)
        {
            if (strstr(dir->d_name, ".col") != NULL)
            {
                char filepath[MAX_FILENAME_LENGTH];
                snprintf(filepath, sizeof(filepath), "./datasets/%s", dir->d_name);
                run_benchmark(filepath);
            }
        }
        closedir(d);
    }
    else
    {
        perror("opendir");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}