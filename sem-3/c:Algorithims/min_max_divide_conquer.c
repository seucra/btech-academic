#include <stdio.h>

/* Structure to store minimum and maximum */
struct MinMax {
    int min;
    int max;
};

/* Function using Divide and Conquer */
struct MinMax findMinMax(int arr[], int low, int high) {
    struct MinMax result, left, right;
    int mid;

    /* If there is only one element */
    if (low == high) {
        result.min = arr[low];
        result.max = arr[low];
        return result;
    }

    /* If there are two elements */
    if (high == low + 1) {
        if (arr[low] < arr[high]) {
            result.min = arr[low];
            result.max = arr[high];
        } else {
            result.min = arr[high];
            result.max = arr[low];
        }
        return result;
    }

    /* More than two elements */
    mid = (low + high) / 2;
    left = findMinMax(arr, low, mid);
    right = findMinMax(arr, mid + 1, high);

    /* Combine results */
    result.min = (left.min < right.min) ? left.min : right.min;
    result.max = (left.max > right.max) ? left.max : right.max;

    return result;
}

int main() {
    int arr[100], n, i;
    struct MinMax result;

    printf("Enter number of elements: ");
    scanf("%d", &n);

    printf("Enter elements:\n");
    for (i = 0; i < n; i++) {
        scanf("%d", &arr[i]);
    }

    result = findMinMax(arr, 0, n - 1);

    printf("Minimum element = %d\n", result.min);
    printf("Maximum element = %d\n", result.max);

    return 0;
}

