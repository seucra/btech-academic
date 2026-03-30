#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define MAX_PAGE_VAL 100 

void print_memory(int memory[], int size) {
    printf("[");
    for (int i = 0; i < size; i++) {
        printf("%d%s", memory[i], (i == size - 1) ? "" : ", ");
    }
    printf("]\n");
}

int find_page(int memory[], int size, int page) {
    for (int i = 0; i < size; i++) {
        if (memory[i] == page) return i;
    }
    return -1;
}

void fifo(int pages[], int n, int frames) {
    printf("\n--- FIFO ---\n");
    int memory[frames];
    int mem_size = 0;
    int front = 0;
    int faults = 0;

    for (int i = 0; i < n; i++) {
        int page = pages[i];
        
        if (find_page(memory, mem_size, page) == -1) {
            if (mem_size < frames) {
                memory[mem_size++] = page;
            } else {
                memory[front] = page;
                front = (front + 1) % frames; // Circular queue logic
            }
            faults++;
            printf("Fault: %d -> ", page);
            print_memory(memory, mem_size);
        } else {
            printf("Hit: %d -> ", page);
            print_memory(memory, mem_size);
        }
    }
    printf("Total FIFO Faults = %d\n", faults);
}

void lru(int pages[], int n, int frames) {
    printf("\n--- LRU ---\n");
    int memory[frames];
    int last_used[frames]; // Tracks the 'time' each frame was last accessed
    int mem_size = 0;
    int faults = 0;
    int time = 0;

    for (int i = 0; i < n; i++) {
        int page = pages[i];
        time++;
        
        int index = find_page(memory, mem_size, page);
        
        if (index == -1) {
            if (mem_size < frames) {
                memory[mem_size] = page;
                last_used[mem_size] = time;
                mem_size++;
            } else {
                // Find least recently used
                int lru_index = 0, min_time = last_used[0];
                for (int j = 1; j < frames; j++) {
                    if (last_used[j] < min_time) {
                        min_time = last_used[j];
                        lru_index = j;
                    }
                }
                memory[lru_index] = page;
                last_used[lru_index] = time;
            }
            faults++;
            printf("Fault: %d -> ", page);
            print_memory(memory, mem_size);
        } else {
            last_used[index] = time; // Update usage time on hit
            printf("Hit: %d -> ", page);
            print_memory(memory, mem_size);
        }
    }
    printf("Total LRU Faults = %d\n", faults);
}

void lfu(int pages[], int n, int frames) {
    printf("\n--- LFU ---\n");
    int memory[frames];
    int mem_size = 0;
    int faults = 0;
    int time = 0;
    
    // Arrays acting as HashMaps
    int freq[MAX_PAGE_VAL] = {0}; 
    int last_used_map[MAX_PAGE_VAL] = {0};

    for (int i = 0; i < n; i++) {
        int page = pages[i];
        time++;
        
        freq[page]++;
        last_used_map[page] = time;
        
        if (find_page(memory, mem_size, page) == -1) {
            if (mem_size < frames) {
                memory[mem_size++] = page;
            } else {
                int min_freq = INT_MAX;
                int oldest_time = INT_MAX;
                int replace_index = -1;

                for (int j = 0; j < mem_size; j++) {
                    int p = memory[j];
                    if (freq[p] < min_freq || (freq[p] == min_freq && last_used_map[p] < oldest_time)) {
                        min_freq = freq[p];
                        oldest_time = last_used_map[p];
                        replace_index = j;
                    }
                }
                memory[replace_index] = page;
            }
            faults++;
            printf("Fault: %d -> ", page);
            print_memory(memory, mem_size);
        } else {
            printf("Hit: %d -> ", page);
            print_memory(memory, mem_size);
        }
    }
    printf("Total LFU Faults = %d\n", faults);
}

void optimal(int pages[], int n, int frames) {
    printf("\n--- OPTIMAL ---\n");
    int memory[frames];
    int mem_size = 0;
    int faults = 0;

    for (int i = 0; i < n; i++) {
        int page = pages[i];
        
        if (find_page(memory, mem_size, page) == -1) {
            if (mem_size < frames) {
                memory[mem_size++] = page;
            } else {
                int farthest = -1;
                int replace_index = -1;

                for (int j = 0; j < mem_size; j++) {
                    int p = memory[j];
                    int k;
                    for (k = i + 1; k < n; k++) {
                        if (p == pages[k]) {
                            break;
                        }
                    }

                    if (k == n) { // Page is never used again
                        replace_index = j;
                        break;
                    }

                    if (k > farthest) {
                        farthest = k;
                        replace_index = j;
                    }
                }
                memory[replace_index] = page;
            }
            faults++;
            printf("Fault: %d -> ", page);
            print_memory(memory, mem_size);
        } else {
            printf("Hit: %d -> ", page);
            print_memory(memory, mem_size);
        }
    }
    printf("Total Optimal Faults = %d\n", faults);
}

int main() {
    int pages[] = {6, 7, 0, 2, 2, 6, 6, 7, 6};
    int n = sizeof(pages) / sizeof(pages[0]);
    int frames = 3;

    printf("Pages: ");
    print_memory(pages, n);
    printf("Frames: %d\n", frames);

    fifo(pages, n, frames);
    lru(pages, n, frames);
    lfu(pages, n, frames);
    optimal(pages, n, frames);

    return 0;
}
