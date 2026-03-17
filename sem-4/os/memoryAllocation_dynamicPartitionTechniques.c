#include <stdio.h>
#include <stdlib.h>

// Node structure for Linked List (Memory Block)
struct Node {
    int blockId;
    double size;  // remaining size
    struct Node* next;
};

// Function to create a new node
struct Node* createNode(int id, double size) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    if (newNode == NULL) {
        printf("Memory allocation failed!\n");
        exit(1);
    }
    newNode->blockId = id;
    newNode->size = size;
    newNode->next = NULL;
    return newNode;
}

// Function to copy the linked list so each algorithm runs independently
struct Node* copyList(struct Node* head) {
    if (head == NULL) return NULL;

    struct Node* newHead = createNode(head->blockId, head->size);
    struct Node* oldTemp = head->next;
    struct Node* newTemp = newHead;

    while (oldTemp != NULL) {
        newTemp->next = createNode(oldTemp->blockId, oldTemp->size);
        oldTemp = oldTemp->next;
        newTemp = newTemp->next;
    }
    return newHead;
}

// Function to free the linked list and prevent memory leaks
void freeList(struct Node* head) {
    struct Node* temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

// Display remaining memory
void display(struct Node* head) {
    struct Node* temp = head;
    while (temp != NULL) {
        printf("Block %d Remaining Memory: %.1fK\n", temp->blockId, temp->size);
        temp = temp->next;
    }
}

// ================= FIRST FIT =================
void firstFit(struct Node* head, double process[], int pCount) {
    printf("\n===== FIRST FIT =====\n");

    for (int i = 0; i < pCount; i++) {
        struct Node* temp = head;
        int allocated = 0;

        while (temp != NULL) {
            if (temp->size >= process[i]) {
                printf("Process P%d (%.1fK) -> Block %d\n", i + 1, process[i], temp->blockId);
                temp->size -= process[i];   // leftover reused
                allocated = 1;
                break;
            }
            temp = temp->next;
        }

        if (!allocated) {
            printf("Process P%d Not Allocated\n", i + 1);
        }
    }
    printf("Remaining Memory:\n");
    display(head);
}

// ================= BEST FIT =================
void bestFit(struct Node* head, double process[], int pCount) {
    printf("\n===== BEST FIT =====\n");

    for (int i = 0; i < pCount; i++) {
        struct Node* temp = head;
        struct Node* best = NULL;

        while (temp != NULL) {
            if (temp->size >= process[i]) {
                if (best == NULL || temp->size < best->size) {
                    best = temp;
                }
            }
            temp = temp->next;
        }

        if (best != NULL) {
            printf("Process P%d (%.1fK) -> Block %d\n", i + 1, process[i], best->blockId);
            best->size -= process[i];
        } else {
            printf("Process P%d Not Allocated\n", i + 1);
        }
    }
    printf("Remaining Memory:\n");
    display(head);
}

// ================= WORST FIT =================
void worstFit(struct Node* head, double process[], int pCount) {
    printf("\n===== WORST FIT =====\n");

    for (int i = 0; i < pCount; i++) {
        struct Node* temp = head;
        struct Node* worst = NULL;

        while (temp != NULL) {
            if (temp->size >= process[i]) {
                if (worst == NULL || temp->size > worst->size) {
                    worst = temp;
                }
            }
            temp = temp->next;
        }

        if (worst != NULL) {
            printf("Process P%d (%.1fK) -> Block %d\n", i + 1, process[i], worst->blockId);
            worst->size -= process[i];
        } else {
            printf("Process P%d Not Allocated\n", i + 1);
        }
    }
    printf("Remaining Memory:\n");
    display(head);
}

// ================= NEXT FIT =================
void nextFit(struct Node* head, double process[], int pCount) {
    printf("\n===== NEXT FIT =====\n");
    if (head == NULL) return;

    struct Node* lastPosition = head;  // starting point

    for (int i = 0; i < pCount; i++) {
        struct Node* temp = lastPosition;
        int allocated = 0;

        do {
            if (temp->size >= process[i]) {
                printf("Process P%d (%.1fK) -> Block %d\n", i + 1, process[i], temp->blockId);
                temp->size -= process[i];
                
                // Move pointer to NEXT block after allocation
                lastPosition = (temp->next == NULL) ? head : temp->next;
                allocated = 1;
                break;
            }
            temp = (temp->next == NULL) ? head : temp->next;

        } while (temp != lastPosition);

        if (!allocated) {
            printf("Process P%d Not Allocated\n", i + 1);
        }
    }
    printf("Remaining Memory:\n");
    display(head);
}

int main() {
    int blocks, p;
    struct Node *head = NULL, *temp = NULL;

    printf("Enter number of memory blocks: ");
    scanf("%d", &blocks);

    for (int i = 1; i <= blocks; i++) {
        double size;
        printf("Enter size of Block %d (in K): ", i);
        scanf("%lf", &size);

        struct Node* newNode = createNode(i, size);

        if (head == NULL) {
            head = newNode;
        } else {
            temp->next = newNode;
        }
        temp = newNode;
    }

    printf("\nEnter number of processes: ");
    scanf("%d", &p);

    // Dynamically allocate array for process sizes
    double* process = (double*)malloc(p * sizeof(double));
    if (process == NULL) {
        printf("Memory allocation failed!\n");
        freeList(head);
        return 1;
    }

    for (int i = 0; i < p; i++) {
        printf("Enter size of Process P%d (in K): ", i + 1);
        scanf("%lf", &process[i]);
    }

    // Run algorithms on independent copies and free them immediately after
    struct Node* copy1 = copyList(head);
    firstFit(copy1, process, p);
    freeList(copy1);

    struct Node* copy2 = copyList(head);
    bestFit(copy2, process, p);
    freeList(copy2);

    struct Node* copy3 = copyList(head);
    worstFit(copy3, process, p);
    freeList(copy3);

    struct Node* copy4 = copyList(head);
    nextFit(copy4, process, p);
    freeList(copy4);

    // Clean up original lists and arrays
    freeList(head);
    free(process);

    return 0;
}
