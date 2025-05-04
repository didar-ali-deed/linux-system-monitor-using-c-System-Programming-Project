#include "../include/sysmonitor.h"
#include <sys/resource.h>

/**
 * @brief Gets detailed statistics for a single process
 * @param p Pointer to ProcessInfo struct to populate
 */
void get_process_stats(ProcessInfo *p) {
    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/stat", p->pid);

    FILE *fp = fopen(path, "r");
    if (fp) {
        unsigned long utime, stime;
        long rss;  // Resident Set Size in pages
        
        // Read relevant fields from /proc/[pid]/stat
        // Note: This skips many fields we don't need (marked with *)
        fscanf(fp, "%*d %*s %c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu %*d %*d %*d %*d %*d %*d %*u %lu",
               &p->state, &utime, &stime, &rss);

        // Calculate CPU usage as percentage
        p->cpu_usage = (utime + stime) / sysconf(_SC_CLK_TCK) / 100.0;
        
        // Calculate memory usage in MB (as double)
        // sysconf(_SC_PAGESIZE) gets system page size in bytes
        p->mem_usage = (rss * sysconf(_SC_PAGESIZE)) / (1024.0 * 1024.0);
        
        fclose(fp);
    }
}

/**
 * @brief Populates array with process information
 * @param processes Array to fill with process data
 * @param max Maximum number of processes to retrieve
 * @return Number of processes actually retrieved
 */
int get_process_list(ProcessInfo *processes, int max) {
    DIR *dir = opendir("/proc");
    struct dirent *entry;
    int count = 0;

    while ((entry = readdir(dir)) != NULL && count < max) {
        // Only look at numeric directory names (process IDs)
        if (entry->d_type == DT_DIR && atoi(entry->d_name) != 0) {
            processes[count].pid = atoi(entry->d_name);
            get_process_stats(&processes[count]);
            count++;
        }
    }
    closedir(dir);
    return count;
}