/*

15-puzzle solver using IDA* (Iterative Deepening A*), which is basically Branch and Bound with iterative deepening and Manhattan heuristic. This is fast in action.

*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define N 4
#define MAX_DEPTH 80

// Goal state
int goal[N][N] = {
    {1, 2, 3, 4},
    {5, 6, 7, 8},
    {9,10,11,12},
    {13,14,15,0}
};

// Directions: up, down, left, right
int dx[4] = {-1,1,0,0};
int dy[4] = {0,0,-1,1};
char move[4] = {'U','D','L','R'};

// Manhattan distance heuristic
int manhattan(int board[N][N]) {
    int dist = 0;
    for(int i=0;i<N;i++){
        for(int j=0;j<N;j++){
            if(board[i][j] != 0){
                int val = board[i][j]-1;
                int goalX = val/N, goalY = val%N;
                dist += abs(i - goalX) + abs(j - goalY);
            }
        }
    }
    return dist;
}

// Check if board equals goal
int is_goal(int board[N][N]){
    for(int i=0;i<N;i++)
        for(int j=0;j<N;j++)
            if(board[i][j] != goal[i][j]) return 0;
    return 1;
}

// Swap two tiles
void swap(int *a, int *b){
    int temp = *a;
    *a = *b;
    *b = temp;
}

// IDA* search
int dfs(int board[N][N], int x, int y, int g, int bound, char path[], int depth){
    int h = manhattan(board);
    int f = g + h;

    if(f > bound) return f;       // prune
    if(h == 0) {                  // goal reached
        path[depth] = '\0';
        printf("Solution: %s\n", path);
        return -1; // found
    }

    int min = 1<<30; // infinity
    for(int i=0;i<4;i++){
        int nx = x + dx[i], ny = y + dy[i];
        if(nx >=0 && nx < N && ny >=0 && ny < N){
            swap(&board[x][y], &board[nx][ny]);
            path[depth] = move[i];
            int t = dfs(board, nx, ny, g+1, bound, path, depth+1);
            if(t == -1) return -1; // solution found
            if(t < min) min = t;
            swap(&board[x][y], &board[nx][ny]); // backtrack
        }
    }
    return min;
}

void ida_star(int board[N][N]){
    int bound = manhattan(board);
    char path[MAX_DEPTH];

    while(1){
        int t = dfs(board, 3,3,0,bound,path,0); // empty tile at (3,3) for start
        if(t == -1) return; // solution printed
        if(t == 1<<30){      // unsolvable
            printf("No solution found!\n");
            return;
        }
        bound = t;           // increase bound
    }
}

int main(){
    int board[N][N] = {
        {1,2,3,4},
        {5,6,7,8},
        {9,10,11,12},
        {13,15,14,0} // Example start
    };

    // Find empty tile position
    int x=0, y=0;
    for(int i=0;i<N;i++)
        for(int j=0;j<N;j++)
            if(board[i][j]==0){ x=i; y=j; break; }

    ida_star(board);

    return 0;
}

