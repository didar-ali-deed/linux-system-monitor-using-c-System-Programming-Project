/**
 * @file stats.c
 * @brief System statistics (CPU, memory) monitoring and process sorting
 */

 #include "../include/sysmonitor.h"
 #include <math.h>
 
 // Static variable to store current sort method
 static SortMethod current_sort_method;
 
 /**
  * @brief Internal comparison function for qsort
  */
 static int compare_processes_wrapper(const void *a, const void *b) {
     const ProcessInfo *pa = (const ProcessInfo *)a;
     const ProcessInfo *pb = (const ProcessInfo *)b;
     
     switch(current_sort_method) {
         case SORT_CPU:
             return (pb->cpu_usage > pa->cpu_usage) ? 1 : -1;
         case SORT_MEM:
             return (pb->mem_usage > pa->mem_usage) ? 1 : -1;
         default:  // SORT_PID
             return pa->pid - pb->pid;
     }
 }
 
 /**
  * @brief Sorts processes array using specified method
  * @param processes Array of ProcessInfo to sort
  * @param count Number of processes in array
  * @param method Sorting method to use
  */
 void sort_processes(ProcessInfo *processes, int count, SortMethod method) {
     current_sort_method = method;
     qsort(processes, count, sizeof(ProcessInfo), compare_processes_wrapper);
 }
 
 /**
  * @brief Gets current CPU usage percentage
  * @param usage Pointer to store CPU usage percentage
  */
 void get_cpu_usage(double *usage) {
     static unsigned long long last_total = 0, last_idle = 0;
     FILE *fp = fopen("/proc/stat", "r");
     if (!fp) {
         perror("fopen");
         return;
     }
 
     char line[256];
     fgets(line, sizeof(line), fp);  // Read first line (cpu stats)
     
     unsigned long long user, nice, system, idle, iowait, irq, softirq;
     sscanf(line + 5, "%llu %llu %llu %llu %llu %llu %llu",
            &user, &nice, &system, &idle, &iowait, &irq, &softirq);
     
     unsigned long long total = user + nice + system + idle + iowait + irq + softirq;
     unsigned long long total_diff = total - last_total;
     unsigned long long idle_diff = idle - last_idle;
     
     *usage = 100.0 * (total_diff - idle_diff) / total_diff;
     
     last_total = total;
     last_idle = idle;
     fclose(fp);
 }
 
 /**
  * @brief Gets memory usage statistics
  * @param total Pointer to store total memory (MB)
  * @param used Pointer to store used memory (MB)
  */
 void get_memory_usage(double *total, double *used) {
     FILE *fp = fopen("/proc/meminfo", "r");
     if (!fp) {
         perror("fopen");
         return;
     }
 
     char line[128];
     long mem_total = 0, mem_free = 0, buffers = 0, cached = 0;
     
     while (fgets(line, sizeof(line), fp)) {
         if (sscanf(line, "MemTotal: %ld kB", &mem_total) == 1) continue;
         if (sscanf(line, "MemFree: %ld kB", &mem_free) == 1) continue;
         if (sscanf(line, "Buffers: %ld kB", &buffers) == 1) continue;
         if (sscanf(line, "Cached: %ld kB", &cached) == 1) continue;
     }
     
     *total = mem_total / 1024.0;  // Convert kB to MB
     *used = (mem_total - mem_free - buffers - cached) / 1024.0;
     fclose(fp);
 }