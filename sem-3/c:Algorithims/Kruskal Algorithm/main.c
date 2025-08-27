#include <stdio.h>
#include <stdlib.h>

// Structure to represent an edge
typedef struct {
    int src, dest, weight;
} Edge;

// Structure to represent a graph
typedef struct {
    int V, E;
    Edge* edge;
} Graph;

typedef struct {
    int parent; // grp leader
    int rank; // tree dept
} Subset;

// Find: recursively finds grp leader of a node 


// Function to create a graph
Graph* createGraph(int V, int E) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    graph->V = V;
    graph->E = E;
    graph->edge = (Edge*)malloc(E * sizeof(Edge));
    return graph;
}

// Function to input graph edges
void inputGraph(Graph* graph) {
    printf("\n:: Enter each edge in the format: source destination weight ::\n");
    for (int i = 0; i < graph->E; i++) {
        printf("Edge %d: ", i);
        scanf("%d %d %d", &graph->edge[i].src, &graph->edge[i].dest, &graph->edge[i].weight);
    }
}

// Function to print graph edges
void printGraph(Graph* graph) {
    printf("\nGraph Edges:\n");
    for (int i = 0; i < graph->E; i++) {
        printf(" %d -> %d == %d\n", graph->edge[i].src, graph->edge[i].dest, graph->edge[i].weight);
    }
}

// Main function
int main() {
    int V, E;
    printf("Enter number of vertices: ");
    scanf("%d", &V);
    printf("Enter number of edges: ");
    scanf("%d", &E);

    Graph* graph = createGraph(V, E);
    inputGraph(graph);
    printGraph(graph);

    // Free dynamically allocated memory
    free(graph->edge);
    free(graph);

    return 0;
}

