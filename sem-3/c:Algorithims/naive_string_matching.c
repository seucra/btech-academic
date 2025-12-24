#include <stdio.h>
#include <string.h>

int main() {
    char text[100], pattern[100];
    int n, m, i, j, found;

    printf("Enter the text: ");
    fgets(text, sizeof(text), stdin);

    printf("Enter the pattern: ");
    fgets(pattern, sizeof(text), stdin);

    text[strcspn(text, "\n")] = '\0';
    pattern[strcspn(pattern, "\n")] = '\0';

    n = strlen(text);
    m = strlen(pattern);

    for (i = 0; i <= n - m; i++) {
        found = 1;
        for (j = 0; j < m; j++) {
            if (text[i + j] != pattern[j]) {
                found = 0;
                break;
            }
        }
        if (found) {
            printf("Pattern found at index %d\n", i);
        }
    }

    return 0;
}

