#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#define MAX_LINE_LENGTH 1000
#define MAX_GRID_LINES 1000

typedef struct Etat {
    char grid[MAX_GRID_LINES][MAX_GRID_LINES];
    int rx, ry;
    struct Etat* next;
} Etat;

Etat* undo_historique = NULL;
Etat* redo_historique = NULL;  

void sauvegarder_etat(char grid[MAX_GRID_LINES][MAX_GRID_LINES], int rx, int ry) {
    Etat* nouvel_etat = malloc(sizeof(Etat)); 
    if (nouvel_etat == NULL) {
        perror("Échec de l'allocation mémoire pour le nouvel état");
        exit(EXIT_FAILURE);
    } 
    for (int i = 0; i < MAX_GRID_LINES; i++) {
        memcpy(nouvel_etat->grid[i], grid[i], MAX_LINE_LENGTH);
    }
    nouvel_etat->rx = rx; 
    nouvel_etat->ry = ry;
    nouvel_etat->next = undo_historique;
    undo_historique = nouvel_etat;

    while (redo_historique != NULL) {
        Etat* temp = redo_historique;
        redo_historique = redo_historique->next; 
        free(temp);
    }
}  

bool undo(char grid[MAX_GRID_LINES][MAX_GRID_LINES], int* rx, int* ry) {
    if (undo_historique == NULL || undo_historique->next == NULL) {
        return false;
    } 

    Etat* current_state_to_redo = undo_historique;
    undo_historique = undo_historique->next; 
    current_state_to_redo->next = redo_historique;
    redo_historique = current_state_to_redo;
 
    for (int i = 0; i < MAX_GRID_LINES; i++) {
        memcpy(grid[i], undo_historique->grid[i], MAX_LINE_LENGTH);
    }
    *rx = undo_historique->rx;
    *ry = undo_historique->ry; 
    
    return true;
} 

bool redo(char grid[MAX_GRID_LINES][MAX_GRID_LINES], int* rx, int* ry) {
    if (redo_historique == NULL) {
        return false;
    }

    Etat* next_state_to_undo = redo_historique;
    redo_historique = redo_historique->next;  
    next_state_to_undo->next = undo_historique;
    undo_historique = next_state_to_undo;

    for (int i = 0; i < MAX_GRID_LINES; i++) {  
        memcpy(grid[i], undo_historique->grid[i], MAX_LINE_LENGTH);
    }
    *rx = undo_historique->rx;
    *ry = undo_historique->ry;  

    return true;
}


void liberer_historique() {
    while (undo_historique != NULL) {
        Etat* temp = undo_historique;  
        undo_historique = undo_historique->next;
        free(temp);
    }
    while (redo_historique != NULL) {
        Etat* temp = redo_historique;
        redo_historique = redo_historique->next;
        free(temp);
    }
}

void printGrid(char grid[MAX_GRID_LINES][MAX_GRID_LINES], int rows) {
    for (int i = 0; i < rows; i++) {
        printf("%s\n", grid[i]);
    }
}

int main() {
    char grid[MAX_GRID_LINES][MAX_GRID_LINES];
    int rows = 15, cols = 15;  
    int rx = -1, ry = -1;
    char move_input;
 

// DEBUT DE FONCTIONNALITE A DECRIRE SOUS FORME DE SCHEMAS
    strcpy(grid[0], "%%%%%%%%%%%%%%%");
    for (int i = 1; i < 14; i++) {
        strcpy(grid[i], "%-------------%");
    }
    strcpy(grid[14], "%%%%%%%%%%%%%%%");
    rows = 15;

    srand(getpid()); 

    int placed_boxes = 0;
    while (placed_boxes < 30) {
        int x = (rand() % 13) + 1;
        int y = (rand() % 13) + 1; 
        if (grid[x][y] == '-') {
            grid[x][y] = '@';
            placed_boxes++;
        }
    }

    int placed_walls = 0;
    while (placed_walls < 15) {
        int x = (rand() % 13) + 1; 
        int y = (rand() % 13) + 1;
        if (grid[x][y] == '-') {
            grid[x][y] = '%';
            placed_walls++;
        } 
    }

    int placed_robot = 0;
    while (placed_robot < 1) { 
        int x = (rand() % 13) + 1;
        int y = (rand() % 13) + 1; 
        if (grid[x][y] == '-') {
            grid[x][y] = 'O';
            rx = x;  
            ry = y; 
            placed_robot++; 
        }
    }
// FIN

// DEBUT DE FONCTIONNALITE A TESTER
    if (rx == -1 || ry == -1) { 
        cols = strlen(grid[0]); 
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
    }
// FIN

    sauvegarder_etat(grid, rx, ry);


    while (true) {
        printGrid(grid, rows);
        printf("Commande (^v<>), 'u' pour annuler, 'r' pour refaire, 'q' pour quitter: ");
        move_input = getchar(); 
        while (getchar() != '\n');

        if (move_input == 'q') { 
            break;
        } else if (move_input == 'u') {
            if (!undo(grid, &rx, &ry)) {
                printf("Impossible d'annuler davantage!\n");
            } 
            continue; 
        } else if (move_input == 'r') {
            if (!redo(grid, &rx, &ry)) {
                printf("Impossible de refaire davantage!\n");
            }
            continue;
        }

        if (move_input != '^' && move_input != 'v' && move_input != '<' && move_input != '>') {
            printf("Commande invalide!\n");
            continue;
        }

        int dx = 0, dy = 0;
        switch (move_input) {
            case '^': dx = -1; break; 
            case 'v': dx = 1; break;
            case '<': dy = -1; break;
            case '>': dy = 1; break; 
        }

        int nx = rx + dx;
        int ny = ry + dy;

        bool move_successful = false;



// 1 DEBUT DE FONCTIONNALITE A TESTER
// 2 DEBUT DE FONCTIONNALITE A DECRIRE SOUS FORME DE SCHEMAS
        if (nx < 0 || nx >= rows || ny < 0 || ny >= cols) {
            printf("Hors limites!\n");
        } else if (grid[nx][ny] == '%') {
            printf("Mur!\n");  
        } else if (grid[nx][ny] == '-') {
            grid[rx][ry] = '-';
            grid[nx][ny] = 'O';
            rx = nx; 
            ry = ny;
            move_successful = true; 
        } else if (grid[nx][ny] == '@') {
            int bx = nx, by = ny;
            int count = 0;
             
            while (bx >= 0 && bx < rows && by >= 0 && by < cols && grid[bx][by] == '@') {
                count++;  
                bx += dx;
                by += dy;
            }

            if (bx < 0 || bx >= rows || by < 0 || by >= cols || grid[bx][by] != '-') {  
                printf("Poussée impossible!\n");
            } else {   
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
                move_successful = true;  
            }
        }
// FIN 1
// FIN 2
 
        if (move_successful) {
            sauvegarder_etat(grid, rx, ry);
        }
    }
 
    liberer_historique();
    return 0;
}

 