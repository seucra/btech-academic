#include <stdio.h>

typedef struct {
    int id;
    int deadline;
    int profit;
} Job;

/* Sort jobs by descending profit */
void sortJobs(Job jobs[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (jobs[i].profit < jobs[j].profit) {
                Job temp = jobs[i];
                jobs[i] = jobs[j];
                jobs[j] = temp;
            }
        }
    }
}

int main() {
    int n;
    printf("Enter number of jobs: ");
    scanf("%d", &n);

    Job jobs[n];
    int maxDeadline = 0;

    printf("Enter job id, deadline, and profit:\n");
    for (int i = 0; i < n; i++) {
        scanf("%d %d %d", &jobs[i].id, &jobs[i].deadline, &jobs[i].profit);
        if (jobs[i].deadline > maxDeadline)
            maxDeadline = jobs[i].deadline;
    }

    sortJobs(jobs, n);

    int slot[maxDeadline + 1];
    for (int i = 0; i <= maxDeadline; i++)
        slot[i] = -1;

    int totalProfit = 0;

    for (int i = 0; i < n; i++) {
        for (int j = jobs[i].deadline; j > 0; j--) {
            if (slot[j] == -1) {
                slot[j] = jobs[i].id;
                totalProfit += jobs[i].profit;
                break;
            }
        }
    }

    printf("Scheduled jobs: ");
    for (int i = 1; i <= maxDeadline; i++)
        if (slot[i] != -1)
            printf("%d ", slot[i]);
    printf("\nTotal Profit: %d\n", totalProfit);

    return 0;
}

