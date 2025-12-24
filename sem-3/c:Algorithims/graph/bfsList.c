#include <stdio.h>
#include <stdlib.h>

#define MAX 20

/* Node for adjacency list */
struct Node {
    int vertex;
    struct Node* next;
};

/* Graph structure */
struct Graph {
    int numVertices;
    struct Node* adjList[MAX];
};

/* Queue for BFS */
int queue[MAX], front = -1, rear = -1;

/* Enqueue */
void enqueue(int v) {
    if (rear == MAX - 1) return;
    if (front == -1) front = 0;
    queue[++rear] = v;
}

/* Dequeue */
int dequeue() {
    if (front == -1 || front > rear) return -1;
    return queue[front++];
}

/* Create new adjacency list node */
struct Node* createNode(int v) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    newNode->vertex = v;
    newNode->next = NULL;
    return newNode;
}

/* Initialize graph */
struct Graph* createGraph(int vertices) {
    struct Graph* graph = (struct Graph*)malloc(sizeof(struct Graph));
    graph->numVertices = vertices;
    for (int i = 0; i < vertices; i++)
        graph->adjList[i] = NULL;
    return graph;
}

/* Add edge (undirected) */
void addEdge(struct Graph* graph, int src, int dest) {
    struct Node* newNode = createNode(dest);
    newNode->next = graph->adjList[src];
    graph->adjList[src] = newNode;

    /* For undirected graph, add edge back */
    newNode = createNode(src);
    newNode->next = graph->adjList[dest];
    graph->adjList[dest] = newNode;
}

/* BFS traversal */
void bfs(struct Graph* graph, int startVertex) {
    int visited[MAX] = {0};
    enqueue(startVertex);
    visited[startVertex] = 1;

    printf("BFS Traversal: ");

    while (front <= rear) {
        int v = dequeue();
        printf("%d ", v);

        struct Node* temp = graph->adjList[v];
        while (temp) {
            int adjVertex = temp->vertex;
            if (!visited[adjVertex]) {
                enqueue(adjVertex);
                visited[adjVertex] = 1;
            }
            temp = temp->next;
        }
    }
    printf("\n");
}

int main() {
    int vertices, edges, src, dest, start;

    printf("Enter number of vertices: ");
    scanf("%d", &vertices);
    struct Graph* graph = createGraph(vertices);

    printf("Enter number of edges: ");
    scanf("%d", &edges);

    printf("Enter edges (src dest):\n");
    for (int i = 0; i < edges; i++) {
        scanf("%d %d", &src, &dest);
        addEdge(graph, src, dest);
    }

    printf("Enter starting vertex for BFS: ");
    scanf("%d", &start);

    bfs(graph, start);

    return 0;
}

