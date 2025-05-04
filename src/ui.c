#include "../include/sysmonitor.h"

/**
 * @brief Initializes ncurses UI
 */
void init_ui() {
    initscr();            // Initialize ncurses
    cbreak();             // Disable line buffering
    noecho();             // Don't echo keypresses
    curs_set(0);          // Hide cursor
    start_color();        // Enable color support
    
    // Set up color pairs
    init_pair(COLOR_DEFAULT, COLOR_WHITE, COLOR_BLACK);
    init_pair(COLOR_HIGHLIGHT, COLOR_GREEN, COLOR_BLACK);
    init_pair(COLOR_WARNING, COLOR_RED, COLOR_BLACK);
    
    timeout(100);         // Non-blocking input with 100ms timeout
}

/**
 * @brief Draws the main UI
 * @param processes Array of process info
 * @param count Number of processes
 * @param sys_cpu System CPU usage percentage
 * @param sys_mem_used Used memory in MB
 * @param sys_mem_total Total memory in MB
 * @param sort_method Current sorting method
 */
void draw_ui(ProcessInfo *processes, int count, double sys_cpu, 
             double sys_mem_used, double sys_mem_total, SortMethod sort_method) {
    clear();  // Clear the screen
    
    // System header
    attron(A_BOLD);
    mvprintw(0, 0, "=== System Monitor (q=quit, c=CPU sort, m=MEM sort) ===");
    attroff(A_BOLD);
    
    // System stats line
    mvprintw(1, 0, "CPU: ");
    attron(COLOR_PAIR(sys_cpu > 70 ? COLOR_WARNING : COLOR_HIGHLIGHT));
    printw("%.1f%%", sys_cpu);
    attroff(COLOR_PAIR(sys_cpu > 70 ? COLOR_WARNING : COLOR_HIGHLIGHT));
    
    mvprintw(1, 20, "Memory: ");
    attron(COLOR_PAIR((sys_mem_used/sys_mem_total) > 0.7 ? COLOR_WARNING : COLOR_HIGHLIGHT));
    printw("%.1f/%.1f MB (%.1f%%)", 
           sys_mem_used, sys_mem_total,
           100 * sys_mem_used / sys_mem_total);
    attroff(COLOR_PAIR((sys_mem_used/sys_mem_total) > 0.7 ? COLOR_WARNING : COLOR_HIGHLIGHT));

    // Column headers
    attron(A_UNDERLINE);
    mvprintw(3, 0, "%-8s%-16s%8s%8s", "PID", "NAME", "CPU%", "MEM(MB)");
    
    // Add sorting indicators if needed
    if (sort_method == SORT_CPU) {
        mvprintw(3, 26, "↓");  // CPU sort indicator
    } else if (sort_method == SORT_MEM) {
        mvprintw(3, 35, "↓");  // MEM sort indicator
    }
    attroff(A_UNDERLINE);
    
    // Process list (show first 20 processes)
    for (int i = 0; i < count && i < 20; i++) {
        // Highlight current process if it's our own
        if (processes[i].pid == getpid()) {
            attron(COLOR_PAIR(COLOR_HIGHLIGHT));
        }
        
        // Print process info with proper formatting
        mvprintw(4 + i, 0, "%-8d%-16s%8.1f%8.1f",
                processes[i].pid,
                processes[i].name,
                processes[i].cpu_usage,
                processes[i].mem_usage);
        
        if (processes[i].pid == getpid()) {
            attroff(COLOR_PAIR(COLOR_HIGHLIGHT));
        }
    }
    
    refresh();  // Update the screen
}

/**
 * @brief Cleans up ncurses resources
 */
void cleanup_ui() {
    endwin();  // Restore terminal to normal state
}