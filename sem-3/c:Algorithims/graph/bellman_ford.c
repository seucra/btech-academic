#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define MAX 20
#define INF INT_MAX

struct Edge {
    int src, dest, weight;
};

int main() {
    int V, E, i, j, src;
    struct Edge edges[MAX];
    int dist[MAX];

    printf("Enter number of vertices: ");
    scanf("%d", &V);

    printf("Enter number of edges: ");
    scanf("%d", &E);

    printf("Enter edges (src dest weight):\n");
    for (i = 0; i < E; i++) {
        scanf("%d %d %d", &edges[i].src, &edges[i].dest, &edges[i].weight);
    }

    printf("Enter source vertex: ");
    scanf("%d", &src);

    /* Initialize distances */
    for (i = 0; i < V; i++)
        dist[i] = INF;
    dist[src] = 0;

    /* Relax all edges V-1 times */
    for (i = 1; i <= V - 1; i++) {
        for (j = 0; j < E; j++) {
            int u = edges[j].src;
            int v = edges[j].dest;
            int w = edges[j].weight;
            if (dist[u] != INF && dist[u] + w < dist[v])
                dist[v] = dist[u] + w;
        }
    }

    /* Check for negative-weight cycles */
    for (i = 0; i < E; i++) {
        int u = edges[i].src;
        int v = edges[i].dest;
        int w = edges[i].weight;
        if (dist[u] != INF && dist[u] + w < dist[v]) {
            printf("Graph contains a negative weight cycle\n");
            return 0;
        }
    }

    /* Print distances */
    printf("Vertex\tDistance from Source %d\n", src);
    for (i = 0; i < V; i++) {
        if (dist[i] == INF)
            printf("%d\tINF\n", i);
        else
            printf("%d\t%d\n", i, dist[i]);
    }

    return 0;
}

