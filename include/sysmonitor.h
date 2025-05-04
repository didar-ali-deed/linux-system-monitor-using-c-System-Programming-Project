#ifndef SYSMONITOR_H
#define SYSMONITOR_H

#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>
#include <math.h>

// Key bindings
#define KEY_QUIT 'q'
#define KEY_SORT_CPU 'c'
#define KEY_SORT_MEM 'm'

// Color Pairs
enum {
    COLOR_DEFAULT = 1,
    COLOR_HIGHLIGHT,
    COLOR_WARNING
};

// Sorting methods
typedef enum {
    SORT_PID,
    SORT_CPU,
    SORT_MEM
} SortMethod;

// Process info structure
typedef struct {
    int pid;         // Process ID
    char name[32];   // Process name
    char state;      // Process state
    double cpu_usage; // CPU usage percentage (as double for precision)
    double mem_usage; // Memory usage in MB (changed from long to double)
} ProcessInfo;

// Function declarations
void list_processes();
void get_cpu_usage(double *usage);
void get_memory_usage(double *total, double *used);
void init_ui();
void draw_ui(ProcessInfo *processes, int count, double sys_cpu, 
             double sys_mem_used, double sys_mem_total, SortMethod sort_method);
void cleanup_ui();
void sort_processes(ProcessInfo *processes, int count, SortMethod method);
int get_process_list(ProcessInfo *processes, int max);

#endif