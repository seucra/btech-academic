#include <time.h>
#include <stdio.h>
#include <stdlib.h>

void insertion (int l[], int n) {
        int i, j, t;
        for (i=1; i<n; i++) {
                for (j=i; j>0; j--) {
                        if (l[j]<l[j-1]) {
                                t = l[j];
                                l[j] = l[j-1];
                                l[j-1] = t;
                        }
                }
        }
}

void fload(int l[]) {
        FILE* file = fopen ("./sortMethordTimeComparision/rando.txt", "r");
        int x, i=0, n=100000;
        while (i<n) {
                fscanf(file, "%d", &x);
                l[i] = x % 100000;
                i++;
        }
	fclose(file);
}

int binarysearch(int l[], int n, int x){
	int low, mid, high;
	low = 0;
	high = n-1;
	while (low < high){
		mid = (low + high)/2;
		if (l[mid] > x){
			high = mid-1;
		}
		else if (l[mid] < x){
			low = mid;
		}
		else // equal
			return mid;
	}
	return -1;
}

int main() {
        int n=100000, i, x;
        double time;
        clock_t start, end;
        int *l = (int *)malloc(n * sizeof(int));

        printf("Loading Array...");
        fload(l);

        printf("\n\n\t::Sorting array for insertion sort::");
        insertion(l,n);

	printf("\n\nEnter integer to search for :: ");
	scanf("%d", &x);

        printf("Starting Binary Search...");
        start = clock();
	i = binarysearch(l, n, x);
        end = clock();
        time = ((double) (end-start)) / CLOCKS_PER_SEC;

	if (i == -1)	printf("\n\t::Given integer '%d' not found in array::", x);
	else		printf("\n\t::Given integer '%d' found at index -> %d::", x, i);

        printf("\nTime Taken for %d: %.4lf", n, time);
	free(l);
	return 0;
}
