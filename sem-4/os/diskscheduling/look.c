#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int compare(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

void run_look() {
    int n, head, seekTime = 0, headIndex = -1;
    int *requests;

    printf("--- LOOK Disk Scheduling ---\n");
    printf("Enter the number of requests: ");
    if (scanf("%d", &n) != 1 || n <= 0) {
        printf("Invalid number of requests.\n");
        return;
    }

    requests = (int *)malloc((n + 1) * sizeof(int));
    if (requests == NULL) return;

    printf("Enter the requests:\n");
    for (int i = 0; i < n; i++) {
        printf("Request %d: ", i + 1);
        scanf("%d", &requests[i]);
    }

    printf("Enter the initial head position: ");
    scanf("%d", &head);

    requests[n] = head;
    int totalElements = n + 1;
    qsort(requests, totalElements, sizeof(int), compare);

    for (int i = 0; i < totalElements; i++) {
        if (requests[i] == head) {
            headIndex = i;
            break;
        }
    }

    printf("\nLOOK Execution Sequence (Moving towards high cylinder first):\n");
    printf("%d", head);

    for (int i = headIndex + 1; i < totalElements; i++) {
        seekTime += abs(requests[i] - head);
        head = requests[i];
        printf(" -> %d", head);
    }

    for (int i = headIndex - 1; i >= 0; i--) {
        seekTime += abs(requests[i] - head);
        head = requests[i];
        printf(" -> %d", head);
    }

    printf("\nTotal head movement is %d\n", seekTime);
    free(requests);
}
int main() {
    run_look();
    return 0;
}

