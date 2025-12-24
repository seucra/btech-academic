/*
TSP Problem: Given n cities and the cost of traveling between each pair, find the shortest possible route that visits each city exactly once and returns to the starting city.

Branch and Bound Approach:

Start from a node (city).

Recursively explore the remaining cities (branch).

Use a lower bound on the path cost to prune non-promising paths (bound).
*/

#include <stdio.h>
#include <limits.h>

#define N 4  // Number of cities

int tsp(int city, int visited, int cost[N][N], int memo[N][1<<N]) {
    if (visited == (1 << N) - 1) // all cities visited
        return cost[city][0];    // return to start city

    if (memo[city][visited] != -1)
        return memo[city][visited];

    int ans = INT_MAX;

    for (int next = 0; next < N; next++) {
        if (!(visited & (1 << next))) { // if city not visited
            int temp = cost[city][next] + tsp(next, visited | (1 << next), cost, memo);
            if (temp < ans) ans = temp;
        }
    }
    return memo[city][visited] = ans;
}

int main() {
    int cost[N][N] = {
        {0, 10, 15, 20},
        {10, 0, 35, 25},
        {15, 35, 0, 30},
        {20, 25, 30, 0}
    };

    int memo[N][1<<N];
    for(int i=0;i<N;i++)
        for(int j=0;j<(1<<N);j++)
            memo[i][j] = -1;

    int result = tsp(0, 1, cost, memo);
    printf("Minimum cost for TSP: %d\n", result);

    return 0;
}

