#include <stdio.h>
#include <stdlib.h>

// adjlist node struct
typedef struct Node {
    int dest;
    struct Node* next;
}Node;

// adj list
typedef struct adjList{
    Node* head;
}adjList;

// graph
typedef struct Graph{
    int v;
    adjList* array;
}Graph;

// create adjlist node
Node* newAdjListNode(int dest) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->dest = dest;
    newNode->next = NULL;
    return newNode;
}

// create graph
Graph* createGraph(int v){
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    graph->v = v;

    graph->array = (adjList*)malloc(v * sizeof(adjList));

    for (int i=0; i<v; i++)
        graph->array[i].head = NULL;

    return graph;
}

//add edge
void addEdge(Graph* graph, int src, int dst){
    // src -> dest
    Node* newNode = newAdjListNode(dst);
    newNode->next = graph->array[src].head;
    graph->array[src].head = newNode;

    // dest -> src
    newNode = newAdjListNode(dst);
    newNode->next = graph->array[dst].head;
    graph->array[dst].head = newNode;
}

void printGraph(Graph* graph) {
    int v;
    for (v=0; v<graph->v; v++) {
        Node* crawl = graph->array[v].head;
        printf("\n Adjency list of vertex %d\n head", v);
        while (crawl) {
            printf("-> %d", crawl->dest);
            crawl = crawl->next;
        }
        printf("\n");
    }
}

int main () {
    int V=5;
    Graph* graph = createGraph(V);
    addEdge(graph, 0, 1);
    addEdge(graph, 0, 4);
    addEdge(graph, 1, 2);
    addEdge(graph, 1, 3);
    addEdge(graph, 1, 4);
    addEdge(graph, 2, 3);
    addEdge(graph, 3, 4);

    printGraph(graph);

    return 0;
}
