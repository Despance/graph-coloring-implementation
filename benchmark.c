// DO NOT USE WHILE COMPILING THE PROJECT
// COMPILE THIS FILE SEPARATELY USING THE FOLLOWING COMMAND
// gcc benchmark.cpp -o benchmark

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // Add this include near other includes

#include <dirent.h>
#include <unistd.h>
#define PATH_SEPARATOR "/"
#define EXECUTABLE_NAME "./graphColoring.out"
#

#define MAX_FILENAME_LENGTH 256
#define MAX_COMMAND_LENGTH 512

#define DATASET_DIR "datasets"

const char *accepted_extensions[] = {".col", ".clq"};
const int num_accepted_extensions = sizeof(accepted_extensions) / sizeof(accepted_extensions[0]);

int has_accepted_extension(const char *filename)
{
    for (int i = 0; i < num_accepted_extensions; ++i)
    {
        if (strstr(filename, accepted_extensions[i]) != NULL)
        {
            return 1;
        }
    }
    return 0;
}

// New helper: trim leading and trailing whitespace in-place
void trim(char *str)
{
    char *start = str;
    while (*start && isspace((unsigned char)*start))
        start++;
    if (start != str)
        memmove(str, start, strlen(start) + 1);
    char *end = str + strlen(str) - 1;
    while (end >= str && isspace((unsigned char)*end))
    {
        *end = '\0';
        end--;
    }
}

// New helper: simple CSV parser that handles quoted fields
int parse_csv_line(const char *line, char tokens[][128], int max_tokens)
{
    int token_index = 0, i = 0, j = 0;
    int in_quotes = 0;
    while (line[i] != '\0' && token_index < max_tokens)
    {
        if (line[i] == '"')
        {
            in_quotes = !in_quotes;
        }
        else if (line[i] == ',' && !in_quotes)
        {
            tokens[token_index][j] = '\0';
            token_index++;
            j = 0;
        }
        else
        {
            tokens[token_index][j++] = line[i];
        }
        i++;
    }
    tokens[token_index][j] = '\0';
    return token_index + 1;
}

int getMaxColorFromFile(const char *solutionName, const char *type)
{
    char filepath[MAX_FILENAME_LENGTH];
    snprintf(filepath, sizeof(filepath), "solutions/%s/sol_%s_%s.txt", solutionName, solutionName, type);

    FILE *file = fopen(filepath, "r");
    if (file == NULL)
    {
        // fprintf(stderr, "File not found: %s\n", filepath);
        return -1;
    }

    int maxColor = -1;

    char line[128];
    while (fgets(line, sizeof(line), file))
    {
        int color = atoi(line);
        if (color > maxColor)
            maxColor = color;
    }
    return maxColor;
}

// Updated helper to retrieve optimal solution from the provided CSV files
int get_optimal_solution(const char *filepath)
{
    // Extract base filename (without extension)
    char base[MAX_FILENAME_LENGTH];
    const char *slash = strrchr(filepath, '/');
#ifdef _WIN32
    if (!slash)
        slash = strrchr(filepath, '\\');
#endif
    if (slash)
        strcpy(base, slash + 1);
    else
        strcpy(base, filepath);
    char *dot = strrchr(base, '.');
    if (dot)
        *dot = '\0';
    trim(base); // Trim extra spaces from the base

    // Array of CSV file paths
    const char *csv_files[] = {
        "datasets/optimals/NP-s.csv",
        "datasets/optimals/NP-m.csv",
        "datasets/optimals/NP-h.csv",
        "datasets/optimals/NP-unknown.csv"};
    int num_csv = sizeof(csv_files) / sizeof(csv_files[0]);

    char line[1024];
    char tokens[20][128];
    for (int i = 0; i < num_csv; i++)
    {
        FILE *fp = fopen(csv_files[i], "r");
        if (!fp)
        {
            perror(csv_files[i]);
            continue;
        }
        // Skip header line
        if (fgets(line, sizeof(line), fp) == NULL)
        {
            fclose(fp);
            continue;
        }
        while (fgets(line, sizeof(line), fp))
        {
            int count = parse_csv_line(line, tokens, 20);
            if (count < 10)
                continue;    // Need at least 10 tokens
            trim(tokens[0]); // Trim instance name from CSV
            // Compare the instance name with the base filename
            if (strcmp(tokens[0], base) == 0)
            {
                int optimal = atoi(tokens[9]);
                fclose(fp);
                return optimal;
            }
        }
        fclose(fp);
    }
    return -1;
}

int optimals = 0;
int betters = 0;
int worsens = 0;
int equals = 0;
int errors = 0;

void run_benchmark(const char *filename)
{
    char command[MAX_COMMAND_LENGTH];
    snprintf(command, sizeof(command), "%s %s", EXECUTABLE_NAME, filename);

    FILE *fp = popen(command, "r");
    if (fp == NULL)
    {
        perror("popen");
        exit(EXIT_FAILURE);
    }

    char output[1024];
    int dsatur = -1;
    int importance_coloring = -1;
    double dsatur_time = -1;
    double importance_time = -1;

    // Extract basename from full path.
    const char *base = strrchr(filename, '/');
    base = base ? base + 1 : filename;

    while (fgets(output, sizeof(output), fp) != NULL)
    {
        double tmp;
        if (sscanf(output, "DSatur-Time: %lf", &tmp) == 1)
            dsatur_time = tmp;
        if (sscanf(output, "Importance-Time: %lf", &tmp) == 1)
            importance_time = tmp;
    }

    importance_coloring = getMaxColorFromFile(base, "importance");
    dsatur = getMaxColorFromFile(base, "dsatur");

    pclose(fp);

    int optimal = get_optimal_solution(filename);

    // Skip the errors
    if (dsatur == -1 || importance_coloring == -1)
    {
        errors++;
        return;
    }

    // Determine ANSI color for Enhanced DSatur column:
    char color[10] = "";

    if (importance_coloring == dsatur)
    {
        strcpy(color, "\033[0m"); // reset
        equals++;
    }
    else if (importance_coloring == optimal)
    {
        strcpy(color, "\033[32m"); // green
        optimals++;
    }
    else if (importance_coloring < dsatur)
    {
        strcpy(color, "\033[33m"); // yellow
        betters++;
    }
    else if (importance_coloring > dsatur)
    {
        strcpy(color, "\033[31m"); // red
        worsens++;
    }
    else
    {
        strcpy(color, "\033[0m"); // reset
        equals++;
    }

    char colored_enhanced[128];
    snprintf(colored_enhanced, sizeof(colored_enhanced), "%s%20d\033[0m", color, importance_coloring);

    // Updated output: color only the Enhanced DSatur column
    printf("%-45s %10d %20s %10d %15.2f ms %15.2f ms\n",
           filename, dsatur, colored_enhanced, optimal, dsatur_time, importance_time);
}

void traverse_directory(const char *dir_path)
{
    DIR *d;
    struct dirent *dir;
    d = opendir(dir_path);

    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            // Skip special directories
            if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
                continue;

            char path[MAX_FILENAME_LENGTH];
            snprintf(path, sizeof(path), "%s/%s", dir_path, dir->d_name);

// If directory, recursively traverse, else check file extension
#ifdef DT_DIR
            if (dir->d_type == DT_DIR)
            {
                traverse_directory(path);
            }
            else
#endif
            {
                if (has_accepted_extension(dir->d_name))
                {
                    run_benchmark(path);
                }
            }
        }
        closedir(d);
    }
    else
    {
        perror("opendir");
    }
}

int main(int argc, char *argv[])
{
    const char *dataset_dir = (argc > 1) ? argv[1] : DATASET_DIR;
    // Updated header: Optimal column is now 4th
    printf("%-45s %10s %20s %10s %18s %18s\n",
           "Filename", "DSatur", "Importance Coloring", "Optimal", "DSatur Time", "Importance Time");
    traverse_directory(dataset_dir);
    printf("Benchmark completed.\n");

    printf("\033[32mOptimals: %d\033[0m\n", optimals); // green
    printf("\033[33mBetters: %d\033[0m\n", betters);   // yellow
    printf("\033[31mWorsens: %d\033[0m\n", worsens);   // red
    printf("\033[0mEquals: %d\033[0m\n", equals);      // reset
    printf("Skipped files: %d\n", errors);
    return 0;
}
