#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// bubble sort
void bubble (int l[], int n) {
	int i, j, t, s;
	for (i=0; i<n; i++) {
		s=0;
		for (j=0; j<n-i-1; j++) {
			if (l[j] > l[j+1]) {
				t = l[j];
				l[j] = l[j+1];
				l[j+1] = t;
				s=1;
			}
		}
		if (s==0)	break;
	}
}

// insertion sort
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

// selection sort
void selection (int l[], int n) {
	int i, j, t, min;
	for (i=0; i<n; i++) {
		min = i;
		for (j=i; j<n; j++) {
			if (l[j] < l[min])	min = j;
		}
		t = l[min];
		l[min] = l[i];
		l[i] = t;
	}
}

// merge sort
void merge(int a[], int dn, int m, int up) {
    int i = dn, j = m+1, k = 0;
    int *temp = (int*)malloc((up-dn+1) * sizeof(int));
    
    while (i<m && j<=up) {
        if (a[i] < a[j])
            temp[k++] = a[i++];
        else
            temp[k++] = a[j++];
    }
    
    while (i<=m)
        temp[k++] = a[i++];
    while (j<=m)
        temp[k++] = a[j++];
    
    for (i=dn, k=0; i<=up; i++)
        a[i] = temp[k++];
    
    free(temp);
}

void mergeSort(int a[], int dn, int up){
    int m;
    if (dn < up) {
        m = (dn+up) / 2;
        mergeSort(a, dn, m);
        mergeSort(a, m+1, up);
        merge(a, dn, m, up);
    }
}

// quick sort
void quickSort(int a[], int lb, int ub) {
    int p;
    if (lb < ub) {
        p = partition(a, lb, ub);
        quickSort(a,lb, p-1);
        quickSort(a, p+1, ub);
    }
}

int partition(int a[], int lb, int ub){
    int dn, up, pivot, temp;
    dn = lb+1;
    up = ub;
    pivot = a[lb];
    while (dn<= up) {
        while (a[up] <= pivot)    dn++;
        while (a[up] > pivot)     up--;
        if (dn < up) {
            temp = a[dn];
            a[dn] = a[up];
            a[up] = temp;
        }
    }
    a[lb] = a[up];
    a[up] = pivot;
    return up;
}


// wrapper for merge and quick for helper function
void merge_sort_wrapper(int *a, int n) {
    mergeSort(a, 0, n - 1);
}

void quick_sort_wrapper(int *a, int n) {
    quickSort(a, 0, n - 1);
}


// helper function
double timeSort(void (*sort_fn)(int *, int), int *arr, int n) {
    clock_t start, end;
    int *copy = (int *)malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) copy[i] = arr[i];

    start = clock();
    sort_fn(copy, n);
    end = clock();

    free(copy);
    return ((double)(end - start)) / CLOCKS_PER_SEC;
}

void fmake (int n) {
	int i=0;
	FILE* file = fopen("rando.txt", "w");
	for (i=0; i<n; i++)	fprintf(file, "%d ", rand());
	fclose(file);
}

void fload(int l[], int n) {
	FILE* file = fopen ("rando.txt", "r");
	int x, i=0;
	while (i<n) {
		fscanf(file, "%d", &x);
		l[i] = x;
		i++;
	}
}

char* fsave (int l[], int n) {
	int i=0;
	FILE* file = fopen("sorto.txt", "w");
	for (i=0; i<n; i++)	fprintf(file, "%d ", l[i]);
	fclose(file);
    return "Done";
}

int main() {
	int n, c;
	double time;
	
    n = 100000;
	int *l = (int *)malloc(n * sizeof(int));

	fmake(n);
	fload(l,n);
    
    printf("Starting Sorting...\n");

    printf("\n\t::Bubble Sort::");
    time = timeSort(bubble, l, n);
    printf("\nTime Taken for %d: %.4lf seconds", n, time);

    printf("\n\t::Insertion Sort::");
    time = time_sort(insertion, l, n);
    printf("\nTime Taken for %d: %.4lf seconds", n, time);

    printf("\n\t::Selection Sort::");
    time = time_sort(selection, l, n);
    printf("\nTime Taken for %d: %.4lf seconds", n, time);

    printf("\n\t::Merge Sort::");
    time = time_sort(merge_sort_wrapper, l, n);
    printf("\nTime Taken for %d: %.4lf seconds", n, time);

    printf("\n\t:Quick Sort::");
    time = time_sort(quick_sort_wrapper, l, n);
    printf("\nTime Taken for %d: %.4lf seconds", n, time);

	printf("Store sorted in file?(0 for no) :: ");
    scanf("%d",&c);
    if (c!=0) {
        quick_sort_wrapper(l,n);
        printf("%s\n", fsave(l,n));
    }

	free(l);
    return 0;
}
