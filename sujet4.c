// Sujet no 4
// 2 fonctionnalités à expliquer en utilisant des schémas
// 2 fonctionnalités à tester
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_LINE_LENGTH 1000
#define MAX_GRID_LINES 1000
#define MAX_MOVES 1000000

void printGrid(char grid[MAX_GRID_LINES][MAX_GRID_LINES], int rows) {
    for (int i = 0; i < rows; i++) {
        printf("%s\n", grid[i]);
    }
}

int main() {
    char grid[MAX_GRID_LINES][MAX_GRID_LINES];
    int rows = 0, cols = 0;
    char moves[MAX_MOVES];
    int moves_len = 0;
    int i, x, y;

// DEBUT DE FONCTIONNALITE A DECRIRE SOUS FORME DE SCHEMAS
    strcpy(grid[0], "%%%%%%%%%%%%%%%");
    for (i=1; i<14 ; i++) {
        strcpy(grid[i], "%-------------%");
    }
    strcpy(grid[14], "%%%%%%%%%%%%%%%");
    rows = 15;
    srand(getpid());
    i=0;
    while(i<30){
        x=(rand()%13)+1;
        y=(rand()%13)+1;
        if (grid[x][y]=='-') {
            grid[x][y]='@';
            i++;
        }
    }
    i=0;
    while(i<15){
        x=(rand()%13)+1;
        y=(rand()%13)+1;
        if (grid[x][y]=='-') {
            grid[x][y]='%';
            i++;
        }
    }
    i=0;
    while(i<1){
        x=(rand()%13)+1;
        y=(rand()%13)+1;
        if (grid[x][y]=='-') {
            grid[x][y]='O';
            i++;
        }
    }
    printGrid(grid, rows);
// FIN

    printf("Enter moves :");
    fgets(moves, sizeof(moves), stdin);
    moves_len = strlen(moves)-1;

    cols = strlen(grid[0]);
    int rx = -1, ry = -1;

// DEBUT DE FONCTIONNALITE A TESTER
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (grid[i][j] == 'O') {
                rx = i;
                ry = j;
                break;
            }
        }
        if (rx != -1) break;
    }
// FIN

// 1 DEBUT DE FONCTIONNALITE A TESTER
// 2 DEBUT DE FONCTIONNALITE A DECRIRE SOUS FORME DE SCHEMAS
    for (int i = 0; i < moves_len; i++) {
        int dx = 0, dy = 0;
        switch(moves[i]) {
            case '^': dx = -1; break;
            case 'v': dx = 1; break;
            case '<': dy = -1; break;
            case '>': dy = 1; break;
        }
        int nx = rx + dx;
        int ny = ry + dy;
        if (nx < 0 || nx >= rows || ny < 0 || ny >= cols) continue;
        if (grid[nx][ny] == '%') {
            continue;
        } else if (grid[nx][ny] == '-') {
            grid[rx][ry] = '-';
            grid[nx][ny] = 'O';
            rx = nx;
            ry = ny;
        } else if (grid[nx][ny] == '@') {
            int count = 0;
            int bx = nx;
            int by = ny;

            while (bx >= 0 && bx < rows && by >= 0 && by < cols && grid[bx][by] == '@') {
                count++;
                bx += dx;
                by += dy;
            }

            if (bx < 0 || bx >= rows || by < 0 || by >= cols || grid[bx][by] != '-') {
                continue;
            }

            for (int j = count - 1; j >= 0; j--) {
                int cx = nx + dx * j;
                int cy = ny + dy * j;
                grid[cx + dx][cy + dy] = '@';
                grid[cx][cy] = '-';
            }

            grid[rx][ry] = '-';
            grid[nx][ny] = 'O';
            rx = nx;
            ry = ny;
        }
    }
// FIN 1
// FIN 2

    printGrid(grid, rows);

    return 0;
}
