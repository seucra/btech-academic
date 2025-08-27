#include <stdio.h>

void swap(int*a int* b) {
	int temp = *a;
	*a = *b;
	*b = temp;
}

int partition(int a[], int low, int high) {
	int pivot = a[high];
	int i = low -1;

	for (int j=low; j<high; j++) {
		if (a[j] < pivor) {
			i++;
			swap(&a[i], &a[j]);
		}
	}
	
	swap(&a[i+1], &a[high]);
	return i+1;
}

void quickSort(int a[], int low, int high) {
	if (low < high) {
		int pi = partition(a, low, high);
		
		quickSort(a, l, pi-1);
		quickSort(a, pi+1, high);
	}
}

void printArray(int a[], int size) {
	for (int i=0; i<size; i++) 	printf("%d ", a[i]);
	printf("\n");
}

int main() {
	int data[] = {8,7,2,1,0,9,6}
	int size = sizeof(data)/ sizeof(data[1]);

	printf("Unsorted Array :: ");
	printArray(data, size);

	quickSort(data, 0, size-1);

	printf("Sorted Array :: ");
	printArray(data, size);

	return 0;
}
