#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

int find_shortest_seek(int *requests, int n, int head, int *visited) {
    int minSeek = INT_MAX;
    int index = -1;

    for (int i = 0; i < n; i++) {
        if (!visited[i]) {
            int seek = abs(requests[i] - head);
            if (seek < minSeek) {
                minSeek = seek;
                index = i;
            }
        }
    }
    return index;
}

void run_sstf() {
    int n, head, seekTime = 0, count = 0;
    int *requests;

    printf("--- SSTF Disk Scheduling ---\n");
    printf("Enter the number of requests: ");
    if (scanf("%d", &n) != 1 || n <= 0) {
        printf("Invalid number of requests.\n");
        return;
    }

    requests = (int *)malloc(n * sizeof(int));
    int *visited = (int *)calloc(n, sizeof(int));
    if (requests == NULL || visited == NULL) return;

    printf("Enter the requests:\n");
    for (int i = 0; i < n; i++) {
        printf("Request %d: ", i + 1);
        scanf("%d", &requests[i]);
    }

    printf("Enter the initial head position: ");
    scanf("%d", &head);

    printf("\nSSTF Execution Sequence:\n");
    printf("%d", head);

    while (count < n) {
        int index = find_shortest_seek(requests, n, head, visited);

        if (index != -1) {
            int seek = abs(requests[index] - head);
            seekTime += seek;
            head = requests[index];
            visited[index] = 1;
            count++;
            printf(" -> %d", head);
        } else {
            break;
        }
    }

    printf("\nTotal head movement is %d\n", seekTime);
    free(requests);
    free(visited);
}
int main() {
    run_sstf();
    return 0;
}

