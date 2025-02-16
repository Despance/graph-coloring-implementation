// DO NOT USE WHILE COMPILING THE PROJECT
// COMPILE THIS FILE SEPARATELY USING THE FOLLOWING COMMAND
// gcc benchmark.cpp -o benchmark

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

#define DATASET_DIR "dataset_INCEA"

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

    // Çıktıyı güncelle
    printf("%-40s %10d %20d %15.2f ms %15.2f ms\n",
           filename, dsatur, enhanced_dsatur, dsatur_time, enhanced_time);
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
        if (!(find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
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
            if (has_accepted_extension(dir->d_name))
            {
                char filepath[MAX_FILENAME_LENGTH];
                snprintf(filepath, sizeof(filepath), "%s/%s", dir_path, dir->d_name);
                run_benchmark(filepath);
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

int main()
{
    const char *dataset_dir = DATASET_DIR;
    printf("%-40s %10s %20s %18s %18s\n",
           "Filename", "DSatur", "Enhanced DSatur", "DSatur Time", "Enhanced Time");
    traverse_directory(dataset_dir);
    return 0;
}
