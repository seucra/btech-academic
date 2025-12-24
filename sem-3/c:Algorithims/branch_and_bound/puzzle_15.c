#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define N 4  // 4x4 board

// Goal state
int goal[N][N] = {
    {1,2,3,4},
    {5,6,7,8},
    {9,10,11,12},
    {13,14,15,0}
};

// Manhattan distance heuristic
int manhattan(int board[N][N]) {
    int dist = 0;
    for(int i=0;i<N;i++) {
        for(int j=0;j<N;j++) {
            if(board[i][j] != 0) {
                int val = board[i][j]-1;
                int goalX = val/N, goalY = val%N;
                dist += abs(i - goalX) + abs(j - goalY);
            }
        }
    }
    return dist;
}

// Check if two boards are same
int is_goal(int board[N][N]) {
    for(int i=0;i<N;i++)
        for(int j=0;j<N;j++)
            if(board[i][j] != goal[i][j])
                return 0;
    return 1;
}

// Directions: up, down, left, right
int dx[4] = {-1,1,0,0};
int dy[4] = {0,0,-1,1};

// Swap tiles
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Branch and Bound solver (DFS with heuristic)
int solve(int board[N][N], int x, int y, int depth, int bound) {
    int h = manhattan(board);
    if(h == 0) return depth; // goal reached
    if(depth + h > bound) return -1; // prune

    for(int i=0;i<4;i++) {
        int nx = x + dx[i], ny = y + dy[i];
        if(nx >=0 && nx < N && ny >=0 && ny < N) {
            swap(&board[x][y], &board[nx][ny]);
            int res = solve(board, nx, ny, depth+1, bound);
            if(res != -1) return res;
            swap(&board[x][y], &board[nx][ny]); // backtrack
        }
    }
    return -1;
}

int main() {
    int board[N][N] = {
        {1,2,3,4},
        {5,6,7,8},
        {9,10,11,12},
        {13,15,14,0} // Example starting board
    };

    int x, y;
    // find empty tile
    for(int i=0;i<N;i++)
        for(int j=0;j<N;j++)
            if(board[i][j]==0) { x=i; y=j; break; }

    int bound = manhattan(board);
    int steps = solve(board, x, y, 0, bound);
    if(steps != -1)
        printf("Solved in %d steps\n", steps);
    else
        printf("Solution not found within bound\n");

    return 0;
}

