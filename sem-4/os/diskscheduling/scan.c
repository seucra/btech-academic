#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int compare(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

void run_scan() {
    int n, head, diskSize, seekTime = 0, headIndex = -1;
    int *requests;
    int direction;

    printf("--- SCAN Disk Scheduling ---\n");
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

    printf("Enter direction (1 for high/up, 0 for low/down): ");
    scanf("%d", &direction);

    requests[n] = head;
    int totalElements = n + 1;
    qsort(requests, totalElements, sizeof(int), compare);

    for (int i = 0; i < totalElements; i++) {
        if (requests[i] == head) {
            headIndex = i;
            break;
        }
    }

    printf("\nSCAN Execution Sequence:\n");
    printf("%d", head);

    if (direction == 1) {
        for (int i = headIndex + 1; i < totalElements; i++) {
            seekTime += abs(requests[i] - head);
            head = requests[i];
            printf(" -> %d", head);
        }

        seekTime += abs(diskSize - head);
        head = diskSize;
        printf(" -> %d (End of Disk)", head);

        for (int i = headIndex - 1; i >= 0; i--) {
            seekTime += abs(requests[i] - head);
            head = requests[i];
            printf(" -> %d", head);
        }
    }
    else {
        for (int i = headIndex - 1; i >= 0; i--) {
            seekTime += abs(requests[i] - head);
            head = requests[i];
            printf(" -> %d", head);
        }

        seekTime += abs(0 - head);
        head = 0;
        printf(" -> %d (End of Disk)", head);

        for (int i = headIndex + 1; i < totalElements; i++) {
            seekTime += abs(requests[i] - head);
            head = requests[i];
            printf(" -> %d", head);
        }
    }

    printf("\nTotal head movement is %d\n", seekTime);
    free(requests);
}
int main() {
    run_scan();
    return 0;
}

