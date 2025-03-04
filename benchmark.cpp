// DO NOT USE WHILE COMPILING THE PROJECT
// COMPILE THIS FILE SEPARATELY USING THE FOLLOWING COMMAND
// gcc benchmark.cpp -o benchmark

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>

#define PATH_SEPARATOR "/"
#define EXECUTABLE_NAME "./graphColoring.out"

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

void generate_unique_filename(char *filename, size_t size)
{
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    snprintf(filename, size, "./benchmark_result/%02d%02d%04d_%02d%02d.txt",
             t->tm_mday, t->tm_mon + 1, t->tm_year + 1900,
             t->tm_hour, t->tm_min);
}

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

void run_benchmark(const char *filename, FILE *output_fp)
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
    fprintf(output_fp, "%-45s %10d %20d %15.2f ms %15.2f ms\n",
            filename, dsatur, enhanced_dsatur, dsatur_time, enhanced_time);
}

void traverse_directory(const char *dir_path, FILE *output_fp)
{
    DIR *d;
    struct dirent *dir;
    d = opendir(dir_path);

    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
                continue;

            char path[MAX_FILENAME_LENGTH];
            snprintf(path, sizeof(path), "%s/%s", dir_path, dir->d_name);

            if (dir->d_type == DT_DIR)
            {
                traverse_directory(path, output_fp);
            }
            else if (has_accepted_extension(dir->d_name))
            {
                run_benchmark(path, output_fp);
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
    char output_filename[64];
    generate_unique_filename(output_filename, sizeof(output_filename));
    
    FILE *output_fp = fopen(output_filename, "w");
    if (!output_fp)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    const char *dataset_dir = (argc > 1) ? argv[1] : DATASET_DIR;
    fprintf(output_fp, "%-45s %10s %20s %18s %18s\n",
            "Filename", "DSatur", "Enhanced DSatur", "DSatur Time", "Enhanced Time");
    traverse_directory(dataset_dir, output_fp);
    fprintf(output_fp, "Benchmark completed.\n");
    fclose(output_fp);
    return 0;
}
