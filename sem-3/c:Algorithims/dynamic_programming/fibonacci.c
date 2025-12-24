#include <stdio.h>

int fibonacci(int n) {
    int f[n + 1];
    f[0] = 0;
    f[1] = 1;

    for (int i = 2; i <= n; i++) {
        f[i] = f[i - 1] + f[i - 2];
    }

    return f[n];
}

int main() {
    int n;
    printf("Enter n: ");
    scanf("%d", &n);

    if (n < 0) {
        printf("Invalid input\n");
        return 0;
    }

    printf("Fibonacci number F(%d) = %d\n", n, fibonacci(n));
    return 0;
}

