#include <stdio.h>
#include <stdlib.h>

int **graph; // adjacency matrix
int *color;

// Function to check if color assignment is safe
int isSafe(int v, int c, int V) {
    for(int i = 0; i < V; i++) {
        if(graph[v][i] && color[i] == c)
            return 0;
    }
    return 1;
}

// Solve graph coloring using backtracking
void solveGraphColoring(int v, int m, int V, int *solutionCount) {
    if(v == V) {
        (*solutionCount)++;
        printf("Solution %d: ", *solutionCount);
        for(int i = 0; i < V; i++)
            printf("%d ", color[i]);
        printf("\n");
        return;
    }

    for(int c = 1; c <= m; c++) {
        if(isSafe(v, c, V)) {
            color[v] = c;
            solveGraphColoring(v + 1, m, V, solutionCount);
            color[v] = 0; // backtrack
        }
    }
}

int main() {
    int V, m;
    printf("Enter number of vertices: ");
    scanf("%d", &V);
    printf("Enter number of colors: ");
    scanf("%d", &m);

    // Dynamic allocation
    graph = (int **)malloc(V * sizeof(int *));
    for(int i = 0; i < V; i++)
        graph[i] = (int *)malloc(V * sizeof(int));

    color = (int *)malloc(V * sizeof(int));

    printf("Enter adjacency matrix (%d x %d):\n", V, V);
    for(int i = 0; i < V; i++) {
        for(int j = 0; j < V; j++) {
            scanf("%d", &graph[i][j]);
        }
        color[i] = 0; // initialize color
    }

    int solutionCount = 0;
    solveGraphColoring(0, m, V, &solutionCount);
    if(solutionCount == 0)
        printf("No solution exists\n");
    else
        printf("Total solutions: %d\n", solutionCount);

    for(int i = 0; i < V; i++)
        free(graph[i]);
    free(graph);
    free(color);

    return 0;
}

