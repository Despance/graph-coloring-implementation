// DO NOT USE WHILE COMPILING THE PROJECT
// COMPILE THIS FILE SEPARATELY USING THE FOLLOWING COMMAND
// gcc benchmark.cpp -o benchmark

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // Add this include near other includes

#ifdef _WIN32
#include <windows.h>
#include <tchar.h>
#include <direct.h>
#define PATH_SEPARATOR "\\"
#define EXECUTABLE_NAME "a.exe"
#define popen _popen
#define pclose _pclose
#else
#include <dirent.h>
#include <unistd.h>
#define PATH_SEPARATOR "/"
#define EXECUTABLE_NAME "./graphColoring.out"
#endif

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
        "datasets/optimals/NP-?.csv"};
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
    int enhanced_dsatur = -1;
    double dsatur_time = -1;
    double enhanced_time = -1;

    while (fgets(output, sizeof(output), fp) != NULL)
    {
        sscanf(output, "DSatur: %d", &dsatur);
        sscanf(output, "Enhanced DSatur: %d", &enhanced_dsatur);
        sscanf(output, "DSatur completed in: %lf ms", &dsatur_time);
        sscanf(output, "enhancedDSatur completed in: %lf ms", &enhanced_time);
    }

    pclose(fp);

    int optimal = get_optimal_solution(filename);

    // Determine ANSI color for Enhanced DSatur column:
    char color[10] = "";
    if (enhanced_dsatur == optimal)
        strcpy(color, "\033[32m"); // green
    else if (enhanced_dsatur < dsatur)
        strcpy(color, "\033[33m"); // yellow
    else if (enhanced_dsatur > dsatur)
        strcpy(color, "\033[31m"); // red
    else
        strcpy(color, "\033[0m"); // reset

    char colored_enhanced[128];
    snprintf(colored_enhanced, sizeof(colored_enhanced), "%s%20d\033[0m", color, enhanced_dsatur);

    // Updated output: color only the Enhanced DSatur column
    printf("%-45s %10d %20s %10d %15.2f ms %15.2f ms\n",
           filename, dsatur, colored_enhanced, optimal, dsatur_time, enhanced_time);
}

void traverse_directory(const char *dir_path)
{
#ifdef _WIN32
    WIN32_FIND_DATA find_file_data;
    HANDLE h_find = INVALID_HANDLE_VALUE;

    char search_path[MAX_FILENAME_LENGTH];
    snprintf(search_path, sizeof(search_path), "%s\\*", dir_path);

    h_find = FindFirstFile(search_path, &find_file_data);

    if (h_find == INVALID_HANDLE_VALUE)
    {
        fprintf(stderr, "Error opening directory: %s\n", dir_path);
        return;
    }

    do
    {
        if (find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            // Check for "." and ".."
            if (strcmp(find_file_data.cFileName, ".") != 0 && strcmp(find_file_data.cFileName, "..") != 0)
            {
                char subdir[MAX_FILENAME_LENGTH];
                snprintf(subdir, sizeof(subdir), "%s\\%s", dir_path, find_file_data.cFileName);
                traverse_directory(subdir);
            }
        }
        else
        {
            if (has_accepted_extension(find_file_data.cFileName))
            {
                char filepath[MAX_FILENAME_LENGTH];
                snprintf(filepath, sizeof(filepath), "%s\\%s", dir_path, find_file_data.cFileName);
                run_benchmark(filepath);
            }
        }
    } while (FindNextFile(h_find, &find_file_data) != 0);

    FindClose(h_find);
#else
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
#endif
}

int main(int argc, char *argv[])
{
    const char *dataset_dir = (argc > 1) ? argv[1] : DATASET_DIR;
    // Updated header: Optimal column is now 4th
    printf("%-45s %10s %20s %10s %18s %18s\n",
           "Filename", "DSatur", "Enhanced DSatur", "Optimal", "DSatur Time", "Enhanced Time");
    traverse_directory(dataset_dir);
    printf("Benchmark completed.\n");
    return 0;
}
