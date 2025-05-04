#include "../include/sysmonitor.h"
#include <sys/resource.h>

int main() {
    init_ui();
    SortMethod current_sort = SORT_PID;
    ProcessInfo processes[100];
    int process_count = 0;
    
    while (1) {
        // Get system stats
        double cpu_usage, mem_total, mem_used;
        get_cpu_usage(&cpu_usage);
        get_memory_usage(&mem_total, &mem_used);
        
        // Get and sort processes
        process_count = get_process_list(processes, 100);
        sort_processes(processes, process_count, current_sort);
        
        // Handle input
        int ch = getch();
        if (ch == KEY_QUIT) break;
        if (ch == KEY_SORT_CPU) current_sort = SORT_CPU;
        if (ch == KEY_SORT_MEM) current_sort = SORT_MEM;
        
        draw_ui(processes, process_count, cpu_usage, mem_used, mem_total, current_sort);
        usleep(200000);
    }
    
    cleanup_ui();
    return 0;
}