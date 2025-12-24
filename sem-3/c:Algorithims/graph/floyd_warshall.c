#include <stdio.h>
#include <limits.h>

#define MAX 20
#define INF 99999  // Representing "no edge"

int main() {
    int n, i, j, k;
    int dist[MAX][MAX];

    printf("Enter number of vertices: ");
    scanf("%d", &n);

    printf("Enter adjacency matrix (use 0 if no edge, except diagonal):\n");
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            scanf("%d", &dist[i][j]);
            if (i != j && dist[i][j] == 0)
                dist[i][j] = INF; // Convert 0 to INF for no edge
        }
    }

    /* Floyd-Warshall algorithm */
    for (k = 0; k < n; k++) {
        for (i = 0; i < n; i++) {
            for (j = 0; j < n; j++) {
                if (dist[i][k] + dist[k][j] < dist[i][j])
                    dist[i][j] = dist[i][k] + dist[k][j];
            }
        }
    }

    /* Print shortest distance matrix */
    printf("All-Pairs Shortest Distance Matrix:\n");
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            if (dist[i][j] == INF)
                printf("INF ");
            else
                printf("%d ", dist[i][j]);
        }
        printf("\n");
    }

    return 0;
}

