// Travelling Salesperson Problem (TSP) using Branch and Bound

#include <stdio.h>
#include <limits.h>

#define MAX 10

int n;
int final_res = INT_MAX;
int final_path[MAX];

void copyToFinal(int curr_path[], int n) {
    for (int i = 0; i < n; i++)
        final_path[i] = curr_path[i];
    final_path[n] = curr_path[0]; // Complete the cycle
}

/* Function to find the minimum edge cost from vertex i */
int firstMin(int adj[MAX][MAX], int i) {
    int min = INT_MAX;
    for (int k = 0; k < n; k++)
        if (adj[i][k] < min && i != k)
            min = adj[i][k];
    return min;
}

/* Function to find the second minimum edge cost from vertex i */
int secondMin(int adj[MAX][MAX], int i) {
    int first = INT_MAX, second = INT_MAX;
    for (int j = 0; j < n; j++) {
        if (i == j) continue;
        if (adj[i][j] <= first) {
            second = first;
            first = adj[i][j];
        } else if (adj[i][j] <= second)
            second = adj[i][j];
    }
    return second;
}

/* Recursive function for branch and bound */
void TSPRec(int adj[MAX][MAX], int curr_bound, int curr_weight, int level, int curr_path[], int visited[]) {
    if (level == n) {
        if (adj[curr_path[level - 1]][curr_path[0]] != 0) {
            int curr_res = curr_weight + adj[curr_path[level - 1]][curr_path[0]];
            if (curr_res < final_res) {
                copyToFinal(curr_path, n);
                final_res = curr_res;
            }
        }
        return;
    }

    for (int i = 0; i < n; i++) {
        if (adj[curr_path[level - 1]][i] != 0 && visited[i] == 0) {
            int temp = curr_bound;
            curr_weight += adj[curr_path[level - 1]][i];

            if (level == 1)
                curr_bound -= ((firstMin(adj, curr_path[level - 1]) + firstMin(adj, i)) / 2);
            else
                curr_bound -= ((secondMin(adj, curr_path[level - 1]) + firstMin(adj, i)) / 2);

            if (curr_bound + curr_weight < final_res) {
                curr_path[level] = i;
                visited[i] = 1;
                TSPRec(adj, curr_bound, curr_weight, level + 1, curr_path, visited);
            }

            curr_weight -= adj[curr_path[level - 1]][i];
            curr_bound = temp;
            for (int j = 0; j < n; j++)
                visited[j] = (j < level) ? 1 : 0;
        }
    }
}

void TSP(int adj[MAX][MAX]) {
    int curr_path[MAX + 1];
    int visited[MAX] = {0};

    int curr_bound = 0;
    for (int i = 0; i < n; i++)
        curr_bound += (firstMin(adj, i) + secondMin(adj, i));
    curr_bound = (curr_bound & 1) ? curr_bound / 2 + 1 : curr_bound / 2;

    visited[0] = 1;
    curr_path[0] = 0;

    TSPRec(adj, curr_bound, 0, 1, curr_path, visited);

    printf("Minimum cost: %d\n", final_res);
    printf("Path Taken: ");
    for (int i = 0; i <= n; i++)
        printf("%d ", final_path[i]);
    printf("\n");
}

int main() {
    int adj[MAX][MAX];

    printf("Enter number of cities (<=10 recommended): ");
    scanf("%d", &n);

    printf("Enter cost adjacency matrix:\n");
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            scanf("%d", &adj[i][j]);

    TSP(adj);

    return 0;
}

