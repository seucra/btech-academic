#include <stdio.h>

#define MAX 20

int visited[MAX];

/* DFS function */
void dfs(int adj[MAX][MAX], int v, int n) {
    visited[v] = 1;
    printf("%d ", v);

    for (int i = 0; i < n; i++) {
        if (adj[v][i] == 1 && !visited[i]) {
            dfs(adj, i, n);
        }
    }
}

int main() {
    int n, adj[MAX][MAX], i, j, start;

    printf("Enter number of vertices: ");
    scanf("%d", &n);

    printf("Enter adjacency matrix:\n");
    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++)
            scanf("%d", &adj[i][j]);

    for (i = 0; i < n; i++)
        visited[i] = 0;

    printf("Enter starting vertex (0 to %d): ", n - 1);
    scanf("%d", &start);

    printf("DFS Traversal: ");
    dfs(adj, start, n);
    printf("\n");

    return 0;
}

