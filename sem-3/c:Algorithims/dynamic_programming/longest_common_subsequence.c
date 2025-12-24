#include <stdio.h>
#include <string.h>

int max(int a, int b) {
    return (a > b) ? a : b;
}

int lcs(char X[], char Y[]) {
    int m = strlen(X);
    int n = strlen(Y);
    int L[m + 1][n + 1];

    /* Build LCS table in bottom-up manner */
    for (int i = 0; i <= m; i++) {
        for (int j = 0; j <= n; j++) {
            if (i == 0 || j == 0)
                L[i][j] = 0;
            else if (X[i - 1] == Y[j - 1])
                L[i][j] = L[i - 1][j - 1] + 1;
            else
                L[i][j] = max(L[i - 1][j], L[i][j - 1]);
        }
    }

    return L[m][n];
}

int main() {
    char X[100], Y[100];

    printf("Enter first string: ");
    scanf("%s", X);
    printf("Enter second string: ");
    scanf("%s", Y);

    int length = lcs(X, Y);
    printf("Length of Longest Common Subsequence = %d\n", length);

    return 0;
}

