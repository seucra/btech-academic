/*
 * Use: Determine the optimal order to multiply matrices to minimize scalar multiplications.

 * Key idea: Multiply matrices in different orders to reduce computation.

 * Applications:

    Computer graphics

    Linear algebra optimization

    Database query optimization
 */


#include <stdio.h>
#include <limits.h>

int min(int a, int b) {
    return (a < b) ? a : b;
}

int matrixChainOrder(int p[], int n) {
    int m[n][n];

    // m[i][j] = Minimum number of multiplications needed to multiply matrices i..j
    for (int i = 1; i < n; i++)
        m[i][i] = 0;

    for (int L = 2; L < n; L++) {  // L = chain length
        for (int i = 1; i < n - L + 1; i++) {
            int j = i + L - 1;
            m[i][j] = INT_MAX;
            for (int k = i; k <= j - 1; k++) {
                int q = m[i][k] + m[k + 1][j] + p[i - 1] * p[k] * p[j];
                if (q < m[i][j])
                    m[i][j] = q;
            }
        }
    }

    return m[1][n - 1];
}

int main() {
    int n;
    printf("Enter number of matrices: ");
    scanf("%d", &n);

    int p[n + 1];
    printf("Enter dimensions (p0 p1 ... pn):\n");
    for (int i = 0; i <= n; i++)
        scanf("%d", &p[i]);

    int minMultiplications = matrixChainOrder(p, n + 1);
    printf("Minimum number of multiplications = %d\n", minMultiplications);

    return 0;
}

