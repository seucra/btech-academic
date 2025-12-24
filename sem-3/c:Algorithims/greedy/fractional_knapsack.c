#include <stdio.h>

typedef struct {
    int weight;
    int value;
    double ratio;
} Item;

/* Sort items by value/weight ratio descending */
void sortItems(Item items[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (items[i].ratio < items[j].ratio) {
                Item temp = items[i];
                items[i] = items[j];
                items[j] = temp;
            }
        }
    }
}

int main() {
    int n, capacity;
    printf("Enter number of items: ");
    scanf("%d", &n);

    Item items[n];
    printf("Enter weight and value of each item:\n");
    for (int i = 0; i < n; i++) {
        scanf("%d %d", &items[i].weight, &items[i].value);
        items[i].ratio = (double)items[i].value / items[i].weight;
    }

    printf("Enter knapsack capacity: ");
    scanf("%d", &capacity);

    sortItems(items, n);

    double totalValue = 0;
    int remaining = capacity;

    for (int i = 0; i < n; i++) {
        if (items[i].weight <= remaining) {
            totalValue += items[i].value;
            remaining -= items[i].weight;
        } else {
            totalValue += items[i].ratio * remaining;
            break;
        }
    }

    printf("Maximum value in knapsack: %.2lf\n", totalValue);

    return 0;
}

