#include <stdio.h>
#include <stdbool.h>

int main() {
    int n, sum;
    printf("Enter number of elements: ");
    scanf("%d", &n);

    int arr[n];
    printf("Enter the elements:\n");
    for (int i = 0; i < n; i++)
        scanf("%d", &arr[i]);

    printf("Enter the target sum: ");
    scanf("%d", &sum);

    bool dp[n + 1][sum + 1];

    // Initialize the DP table
    for (int i = 0; i <= n; i++) {
        for (int j = 0; j <= sum; j++) {
            if (j == 0)
                dp[i][j] = true;  // sum 0 is always possible
            else if (i == 0)
                dp[i][j] = false; // no elements cannot make positive sum
            else if (arr[i - 1] <= j)
                dp[i][j] = dp[i - 1][j] || dp[i - 1][j - arr[i - 1]];
            else
                dp[i][j] = dp[i - 1][j];
        }
    }

    if (dp[n][sum])
        printf("Subset with sum %d exists.\n", sum);
    else
        printf("Subset with sum %d does not exist.\n", sum);

    return 0;
}

