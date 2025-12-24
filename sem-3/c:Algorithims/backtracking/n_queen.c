#include <stdio.h>
#include <stdlib.h>

// Function to print the board
void printBoard(int *board, int N) {
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++)
            printf("%c ", (board[i] == j) ? 'Q' : '.');
        printf("\n");
    }
    printf("\n");
}

// Check if placing queen at row i, column j is safe
int isSafe(int *board, int row, int col) {
    for(int i = 0; i < col; i++) {
        if(board[i] == row || abs(board[i] - row) == abs(i - col))
            return 0;
    }
    return 1;
}

// Solve N-Queen using backtracking
void solveNQueen(int *board, int col, int N, int *solutionCount) {
    if(col == N) {
        (*solutionCount)++;
        printBoard(board, N);
        return;
    }

    for(int row = 0; row < N; row++) {
        if(isSafe(board, row, col)) {
            board[col] = row;
            solveNQueen(board, col + 1, N, solutionCount);
            board[col] = -1; // backtrack
        }
    }
}

int main() {
    int N;
    printf("Enter number of queens: ");
    scanf("%d", &N);

    int *board = (int *)malloc(N * sizeof(int));
    for(int i = 0; i < N; i++)
        board[i] = -1;

    int solutionCount = 0;
    solveNQueen(board, 0, N, &solutionCount);
    printf("Total solutions: %d\n", solutionCount);

    free(board);
    return 0;
}

