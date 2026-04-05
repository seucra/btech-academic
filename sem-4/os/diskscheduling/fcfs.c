#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void run_fcfs() {
    int n, head, seekTime = 0, currentTrack;
    int *requests;

    printf("--- FCFS Disk Scheduling ---\n");
    printf("Enter the number of requests: ");
    if (scanf("%d", &n) != 1 || n <= 0) {
        printf("Invalid number of requests.\n");
        return;
    }

    requests = (int *)malloc(n * sizeof(int));
    if (requests == NULL) return;

    printf("Enter the requests:\n");
    for (int i = 0; i < n; i++) {
        printf("Request %d: ", i + 1);
        scanf("%d", &requests[i]);
    }

    printf("Enter the initial head position: ");
    scanf("%d", &head);

    printf("\nFCFS Execution Sequence:\n");
    printf("%d", head);

    for (int i = 0; i < n; i++) {
        currentTrack = requests[i];
        seekTime += abs(currentTrack - head);
        head = currentTrack;
        printf(" -> %d", head);
    }

    printf("\nTotal head movement is %d\n", seekTime);
    free(requests);
}
int main() {
    run_fcfs();
    return 0;
}

