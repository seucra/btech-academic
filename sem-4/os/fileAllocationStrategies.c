#include <stdio.h>
#include <stdlib.h>
#define MAX 50
int memory[MAX];
struct node {
    int block;
    struct node* next;
};

// Initialize memory
void initialize() {
    for(int i = 0; i < MAX; i++)
        memory[i] = 0;
}

// Display memory
void display() {
    printf("\nMemory Blocks:\n");
    for(int i = 0; i < MAX; i++)
        printf("%d ", memory[i]);
    printf("\n");
}

/* ---------------- CONTIGUOUS ---------------- */
void contiguous() {
    int start, length, flag = 0;

    printf("\nEnter starting block: ");
    scanf("%d", &start);
    printf("Enter length: ");
    scanf("%d", &length);

    if(start + length > MAX) {
        printf("Invalid range!\n");
        return;
    }

    for(int i = start; i < start + length; i++) {
        if(memory[i] == 1) {
            flag = 1;
            break;
        }
    }

    if(flag) {
        printf("Allocation not possible!\n");
    } else {
        for(int i = start; i < start + length; i++)
            memory[i] = 1;

        printf("Contiguous Allocation: ");
        for(int i = start; i < start + length; i++)
            printf("%d ", i);
    }
}

/* ---------------- LINKED ---------------- */
void linked() {
    int n, block;

    struct node *head = NULL, *temp = NULL, *newNode;

    printf("\nEnter number of blocks: ");
    scanf("%d", &n);

    for(int i = 0; i < n; i++) {
        printf("Enter block %d: ", i+1);
        scanf("%d", &block);

        if(block >= MAX || memory[block] == 1) {
            printf("Block %d invalid or already allocated!\n", block);
            // Cleanup on error
            while(head != NULL) {
                temp = head;
                head = head->next;
                free(temp);
            }
            return;
        }
        memory[block] = 1;
        newNode = (struct node*)malloc(sizeof(struct node));
        newNode->block = block;
        newNode->next = NULL;
        if(head == NULL) {
            head = newNode;
            temp = newNode;
        } else {
            temp->next = newNode;
            temp = newNode;
        }
    }
    printf("Linked Allocation : ");
    temp = head;
    while(temp != NULL) {
        printf("%d -> ", temp->block);
        temp = temp->next;
    }
    printf("NULL\n");
    // Cleanup after display
    while(head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

/* ---------------- INDEXED ---------------- */
void indexed() {
    int indexBlock, n;

    printf("\nEnter index block: ");
    scanf("%d", &indexBlock);

    if(indexBlock >= MAX || memory[indexBlock] == 1) {
        printf("Index block invalid or already allocated!\n");
        return;
    }

    printf("Enter number of blocks: ");
    scanf("%d", &n);

    int indexArray[n];

    memory[indexBlock] = 1;

    for(int i = 0; i < n; i++) {
        printf("Enter block %d: ", i+1);
        scanf("%d", &indexArray[i]);

        if(indexArray[i] >= MAX || memory[indexArray[i]] == 1) {
            printf("Block invalid or already allocated!\n");
            memory[indexBlock] = 0;
            return;
        }

        memory[indexArray[i]] = 1;
    }

    printf("Indexed Allocation:\nIndex Block: %d\nBlocks: ", indexBlock);
    for(int i = 0; i < n; i++)
        printf("%d ", indexArray[i]);

    printf("\n");
}

int main() {
    int choice;

    initialize();

    while(1) {
        printf("\n--- File Allocation ---\n");
        printf("1. Contiguous Allocation\n");
        printf("2. Linked Allocation\n");
        printf("3. Indexed Allocation\n");
        printf("4. Display Memory\n");
        printf("5. Exit\n");

        printf("Enter choice: ");
        scanf("%d", &choice);

        switch(choice) {
            case 1: contiguous(); break;
            case 2: linked(); break;
            case 3: indexed(); break;
            case 4: display(); break;
            case 5: exit(0);
            default: printf("Invalid choice!\n");
        }
    }
}

