#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int compare(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

void run_cscan() {
    int n, head, diskSize, seekTime = 0, headIndex = -1;
    int *requests;

    printf("--- C-SCAN Disk Scheduling ---\n");
    printf("Enter the number of requests: ");
    if (scanf("%d", &n) != 1 || n <= 0) {
        printf("Invalid number of requests.\n");
        return;
    }

    requests = (int *)malloc((n + 1) * sizeof(int));
    if (requests == NULL) return;

    printf("Enter the requests (0 to max cylinder):\n");
    for (int i = 0; i < n; i++) {
        printf("Request %d: ", i + 1);
        scanf("%d", &requests[i]);
    }

    printf("Enter the initial head position: ");
    scanf("%d", &head);

    printf("Enter the maximum cylinder number (e.g., 199 for 0-199): ");
    scanf("%d", &diskSize);

    requests[n] = head;
    int totalElements = n + 1;
    qsort(requests, totalElements, sizeof(int), compare);

    for (int i = 0; i < totalElements; i++) {
        if (requests[i] == head) {
            headIndex = i;
            break;
        }
    }

    printf("\nC-SCAN Execution Sequence (Moving towards high cylinder):\n");
    printf("%d", head);

    for (int i = headIndex + 1; i < totalElements; i++) {
        seekTime += abs(requests[i] - head);
        head = requests[i];
        printf(" -> %d", head);
    }

    seekTime += abs(diskSize - head);
    head = diskSize;
    printf(" -> %d (Max Disk End)", head);

    seekTime += abs(head - 0);
    head = 0;
    printf(" -> %d (Jump to Min Disk End)", head);

    for (int i = 0; i < headIndex; i++) {
        seekTime += abs(requests[i] - head);
        head = requests[i];
        printf(" -> %d", head);
    }

    printf("\nTotal head movement is %d\n", seekTime);
    free(requests);
}
int main() {
    run_cscan();
    return 0;
}

