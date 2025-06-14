#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#define MAX_GRID_LINES 1000
#define MAX_LINE_LENGTH 1000

typedef struct Etat {
    char grid[MAX_GRID_LINES][MAX_GRID_LINES];
    int rx, ry;  
    struct Etat* next;  
} Etat;
 
Etat* test_undo_historique = NULL; 
Etat* test_redo_historique = NULL;

void test_sauvegarder_etat(char grid[MAX_GRID_LINES][MAX_GRID_LINES], int rx, int ry) {    
    Etat* nouvel_etat = malloc(sizeof(Etat));    
    if (nouvel_etat == NULL) {
        perror("Échec de l'allocation mémoire pour le nouvel état lors de la sauvegarde de test");      
        exit(EXIT_FAILURE); 
    }
    for (int i = 0; i < MAX_GRID_LINES; i++) {
        memcpy(nouvel_etat->grid[i], grid[i], MAX_LINE_LENGTH);
    }
    nouvel_etat->rx = rx; 
    nouvel_etat->ry = ry;     
    nouvel_etat->next = test_undo_historique;
    test_undo_historique = nouvel_etat;
 
    while (test_redo_historique != NULL) { 
        Etat* temp = test_redo_historique;
        test_redo_historique = test_redo_historique->next;
        free(temp);
    }
} 

bool test_undo(char grid[MAX_GRID_LINES][MAX_GRID_LINES], int* rx, int* ry) {
    if (test_undo_historique == NULL || test_undo_historique->next == NULL) {
        return false;
    }

    Etat* current_state = test_undo_historique;
    test_undo_historique = test_undo_historique->next;  
    current_state->next = test_redo_historique;  
    test_redo_historique = current_state;

    for (int i = 0; i < MAX_GRID_LINES; i++) {
        memcpy(grid[i], test_undo_historique->grid[i], MAX_LINE_LENGTH);
    }
    *rx = test_undo_historique->rx;
    *ry = test_undo_historique->ry;  
      
    return true;
} 
  
bool test_redo(char grid[MAX_GRID_LINES][MAX_GRID_LINES], int* rx, int* ry) {
    if (test_redo_historique == NULL) {
        return false;
    } 

    Etat* next_state = test_redo_historique; 
    test_redo_historique = test_redo_historique->next; 
    next_state->next = test_undo_historique;
    test_undo_historique = next_state;
 
    for (int i = 0; i < MAX_GRID_LINES; i++) {
        memcpy(grid[i], test_undo_historique->grid[i], MAX_LINE_LENGTH);
    }
    *rx = test_undo_historique->rx;
    *ry = test_undo_historique->ry;

    return true;
}

void test_liberer_historique() {
    while (test_undo_historique != NULL) {
        Etat* temp = test_undo_historique;
        test_undo_historique = test_undo_historique->next;
        free(temp);
    }
    while (test_redo_historique != NULL) {
        Etat* temp = test_redo_historique;
        test_redo_historique = test_redo_historique->next;
        free(temp);
    }
}


void printGrid(char grid[MAX_GRID_LINES][MAX_LINE_LENGTH], int rows) {
    for (int i = 0; i < rows; i++) {
        printf("%s\n", grid[i]);
    }
}

bool simulate_move(char grid[MAX_GRID_LINES][MAX_LINE_LENGTH], int rows, int cols, int* rx, int* ry, char move_char) {
    int dx = 0, dy = 0;
    switch(move_char) {
        case '^': dx = -1; break;
        case 'v': dx = 1; break;
        case '<': dy = -1; break;
        case '>': dy = 1; break;
        default: return false; 
    }
    int nx = *rx + dx, ny = *ry + dy;

    if (nx < 0 || nx >= rows || ny < 0 || ny >= cols) {
        return false; 
    }
    if (grid[nx][ny] == '%') {
        return false; 
    } else if (grid[nx][ny] == '-') {
        grid[*rx][*ry] = '-';
        grid[nx][ny] = 'O';
        *rx = nx;
        *ry = ny;
        return true;
    } else if (grid[nx][ny] == '@') {
        int bx = nx, by = ny;
        int count = 0;
        while (bx >= 0 && bx < rows && by >= 0 && by < cols && grid[bx][by] == '@') {
            count++;
            bx += dx;
            by += dy;
        }
        if (bx < 0 || bx >= rows || by < 0 || by >= cols || grid[bx][by] != '-') {
            return false; 
        }
        for (int j = count - 1; j >= 0; j--) {
            int cx = nx + dx * j;
            int cy = ny + dy * j;
            grid[cx + dx][cy + dy] = '@';
            grid[cx][cy] = '-';
        }
        grid[*rx][*ry] = '-';
        grid[nx][ny] = 'O';
        *rx = nx;
        *ry = ny;
        return true;
    }
    return false; 
} 

// Tests pour la fonctionnalité 1 : Détection du robot
void test_find_robot() { 
    char grid[MAX_GRID_LINES][MAX_LINE_LENGTH] = {
        "%%%%%%%%%%%%%%%",
        "%-------------%",  
        "%---O---------%",  // Robot en (2, 4)
        "%-------------%",
        "%%%%%%%%%%%%%%%"
    };
    int rows = 5;
    int cols = strlen(grid[0]);
    int rx = -1, ry = -1;

    // Recherche du robot   
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
    assert(rx == 2 && ry == 4);
    printf("Test find_robot: OK (Robot trouvé à (%d, %d))\n", rx, ry);
}

// Test pour la fonctionnalité 2 : Mouvement du robot vers un espace vide
void test_move_to_empty() {
    char grid[MAX_GRID_LINES][MAX_LINE_LENGTH] = {
        "%%%%%%%%%%%%%%%",
        "%-------------%",
        "%---O---------%",
        "%-------------%",
        "%%%%%%%%%%%%%%%"
    };
    int rows = 5;
    int cols = strlen(grid[0]);
    int rx = 2, ry = 4; // Position initiale du robot

    bool success = simulate_move(grid, rows, cols, &rx, &ry, '>'); // Déplacer à droite 

    assert(success);
    assert(grid[2][4] == '-'); // L'ancienne position est vide
    assert(grid[2][5] == 'O'); // Le robot s'est déplacé vers la nouvelle position    
    assert(rx == 2 && ry == 5); // Coordonnées du robot mises à jour 
    printf("Test move_to_empty: OK\n");
}
 
// Test pour la fonctionnalité 2 : Pousser une seule caisse
void test_push_single_box() {
    char grid[MAX_GRID_LINES][MAX_LINE_LENGTH] = {
        "%%%%%%%%%%%%%%%",
        "%-------------%",
        "%---O@--------%",  // Robot en (2, 4), caisse en (2, 5)  
        "%-------------%",
        "%%%%%%%%%%%%%%%"
    };
    char expected_grid[MAX_GRID_LINES][MAX_LINE_LENGTH] = {  
        "%%%%%%%%%%%%%%%",
        "%-------------%", 
        "%----O@-------%",  // Attendu : robot en (2,5), caisse en (2,6)
        "%-------------%", 
        "%%%%%%%%%%%%%%%"
    };
    int rows = 5; 
    int cols = strlen(grid[0]); 
    int rx = 2, ry = 4;  

    bool success = simulate_move(grid, rows, cols, &rx, &ry, '>'); //

    assert(success); 
    assert(grid[2][4] == '-');
    assert(grid[2][5] == 'O'); 
    assert(grid[2][6] == '@');
    assert(rx == 2 && ry == 5);   
    for(int i=0; i<rows; ++i) assert(strcmp(grid[i], expected_grid[i]) == 0); 
    printf("Test push_single_box: OK\n");
}
 
// Test pour la fonctionnalité 2 : Pousser plusieurs caisses
void test_push_multiple_boxes() {
    char grid[MAX_GRID_LINES][MAX_LINE_LENGTH] = {
        "%%%%%%%%%%%%%%%",
        "%-------------%",
        "%---O@@-------%",  // Robot en (2, 4), caisses en (2, 5), (2, 6)
        "%-------------%",
        "%%%%%%%%%%%%%%%"
    };
    char expected_grid[MAX_GRID_LINES][MAX_LINE_LENGTH] = {
        "%%%%%%%%%%%%%%%",
        "%-------------%",
        "%----O@@------%",  // Attendu : robot en (2,5), caisses en (2,6), (2,7)
        "%-------------%",
        "%%%%%%%%%%%%%%%"
    };
    int rows = 5;
    int cols = strlen(grid[0]);
    int rx = 2, ry = 4;

    bool success = simulate_move(grid, rows, cols, &rx, &ry, '>');  
    assert(success);
    assert(grid[2][4] == '-');
    assert(grid[2][5] == 'O');
    assert(grid[2][6] == '@');
    assert(grid[2][7] == '@');
    assert(rx == 2 && ry == 5);
    for(int i=0; i<rows; ++i) assert(strcmp(grid[i], expected_grid[i]) == 0);
    printf("Test push_multiple_boxes: OK\n");
}
 
// Test pour la fonctionnalité 2 : Impossible de pousser si bloqué par un mur
void test_push_blocked_by_wall() {
    char grid[MAX_GRID_LINES][MAX_LINE_LENGTH] = { 
        "%%%%%%%%%%%%%%%",
        "%-------------%", 
        "%---O@%-------%",  // Robot en (2, 4), caisse en (2, 5), mur en (2, 6)
        "%-------------%",
        "%%%%%%%%%%%%%%%"
    };
    char initial_grid[MAX_GRID_LINES][MAX_LINE_LENGTH]; 
    memcpy(initial_grid, grid, MAX_GRID_LINES * MAX_LINE_LENGTH); // Sauvegarder l'état initial
    int rows = 5;
    int cols = strlen(grid[0]); 
    int rx = 2, ry = 4;  

    bool success = simulate_move(grid, rows, cols, &rx, &ry, '>');

    assert(!success); // Le mouvement devrait échouer
    // La grille et la position du robot doivent rester inchangées
    assert(rx == 2 && ry == 4);
    for(int i=0; i<rows; ++i) assert(strcmp(grid[i], initial_grid[i]) == 0);
    printf("Test push_blocked_by_wall: OK\n");
}

// Test pour la fonctionnalité 2 : Impossible de se déplacer dans un mur
void test_move_into_wall() {
    char grid[MAX_GRID_LINES][MAX_LINE_LENGTH] = {
        "%%%%%%%%%%%%%%%",
        "%-------------%",
        "%---O%--------%",  // Robot en (2, 4), mur en (2, 5)
        "%-------------%",
        "%%%%%%%%%%%%%%%"
    };
    char initial_grid[MAX_GRID_LINES][MAX_LINE_LENGTH];
    memcpy(initial_grid, grid, MAX_GRID_LINES * MAX_LINE_LENGTH);
    int rows = 5;
    int cols = strlen(grid[0]);
    int rx = 2, ry = 4;

    bool success = simulate_move(grid, rows, cols, &rx, &ry, '>');

    assert(!success); // Le mouvement devrait échouer
    assert(rx == 2 && ry == 4);
    for(int i=0; i<rows; ++i) assert(strcmp(grid[i], initial_grid[i]) == 0);
    printf("Test move_into_wall: OK\n");
}

// Test pour la fonctionnalité 2 : Impossible de se déplacer hors limites
void test_move_out_of_bounds() {
    char grid[MAX_GRID_LINES][MAX_LINE_LENGTH] = {
        "%O------------%", // Robot en (0, 1)
        "%-------------%",
        "%-------------%",
        "%-------------%",
        "%%%%%%%%%%%%%%%"
    }; 
    char initial_grid_up[MAX_GRID_LINES][MAX_LINE_LENGTH];  
    memcpy(initial_grid_up, grid, MAX_GRID_LINES * MAX_LINE_LENGTH); 
    int rows = 5;
    int cols = strlen(grid[0]); 
    int rx_up = 0, ry_up = 1; 

    bool success_up = simulate_move(grid, rows, cols, &rx_up, &ry_up, '^'); // Essayer de monter depuis la première ligne

    assert(!success_up); // Le mouvement devrait échouer
    assert(rx_up == 0 && ry_up == 1);
    for(int i=0; i<rows; ++i) assert(strcmp(grid[i], initial_grid_up[i]) == 0);
    printf("Test move_out_of_bounds (haut): OK\n");

    // Réinitialiser la grille et les coordonnées pour le test vers le bas
    strcpy(grid[0], "%%%%%%%%%%%%%%%");
    for (int i = 1; i < 4; i++) {
        strcpy(grid[i], "%-------------%");
    }
    strcpy(grid[4], "%O------------%"); // Placer le robot en (4,1) pour le test vers le bas
    char initial_grid_down[MAX_GRID_LINES][MAX_LINE_LENGTH];
    memcpy(initial_grid_down, grid, MAX_GRID_LINES * MAX_LINE_LENGTH);
    int rx_down = 4, ry_down = 1;

    bool success_down = simulate_move(grid, rows, cols, &rx_down, &ry_down, 'v'); // Essayer de descendre depuis la dernière ligne
    assert(!success_down); // Le mouvement devrait échouer
    assert(rx_down == 4 && ry_down == 1);
    for(int i=0; i<rows; ++i) assert(strcmp(grid[i], initial_grid_down[i]) == 0);
    printf("Test move_out_of_bounds (bas): OK\n");
}
   
// Tests pour la nouvelle fonctionnalité : Annuler/Rétablir
void test_undo_simple() {
    char grid[MAX_GRID_LINES][MAX_LINE_LENGTH] = {
        "%%%%%%%%%%%%%%%",
        "%-------------%",
        "%---O---------%",
        "%-------------%",
        "%%%%%%%%%%%%%%%"
    };
    int rows = 5;
    int cols = strlen(grid[0]); 
    int rx = 2, ry = 4;

    test_liberer_historique(); // Vider l'historique de test précédent 
    test_sauvegarder_etat(grid, rx, ry); // État initial (S0)

    // Mouvement 1 (S0 -> S1) 
    assert(simulate_move(grid, rows, cols, &rx, &ry, '>')); 
    test_sauvegarder_etat(grid, rx, ry); // Sauvegarde S1
  
    // Mouvement 2 (S1 -> S2) 
    assert(simulate_move(grid, rows, cols, &rx, &ry, '>'));
    test_sauvegarder_etat(grid, rx, ry); // Sauvegarde S2
 
    // Annuler le dernier mouvement (S2 -> S1)
    assert(test_undo(grid, &rx, &ry));
    assert(grid[2][5] == 'O'); // Le robot doit être de retour à (2,5)
    assert(rx == 2 && ry == 5);
    printf("Test undo_simple (1ère annulation): OK\n");  

    // Annuler un autre mouvement (S1 -> S0)
    assert(test_undo(grid, &rx, &ry)); 
    assert(grid[2][4] == 'O'); // Le robot doit être de retour à (2,4) (état initial)
    assert(rx == 2 && ry == 4);
    printf("Test undo_simple (2ème annulation, retour à l'initial): OK\n");
    
    // Essayer d'annuler à nouveau (devrait échouer car il ne reste que l'état initial sur la pile)    
    assert(!test_undo(grid, &rx, &ry)); 
    printf("Test undo_simple (plus d'annulation possible): OK\n"); 

    test_liberer_historique();
}

void test_redo_simple() { 
    char grid[MAX_GRID_LINES][MAX_LINE_LENGTH] = {
        "%%%%%%%%%%%%%%%",
        "%-------------%",
        "%---O---------%",
        "%-------------%",
        "%%%%%%%%%%%%%%%"
    };
    int rows = 5;
    int cols = strlen(grid[0]);
    int rx = 2, ry = 4;

    test_liberer_historique();
    test_sauvegarder_etat(grid, rx, ry); // S0

    // Mouvement 1 (S0 -> S1)   
    assert(simulate_move(grid, rows, cols, &rx, &ry, '>'));    
    test_sauvegarder_etat(grid, rx, ry); // S1

    // Mouvement 2 (S1 -> S2)  
    assert(simulate_move(grid, rows, cols, &rx, &ry, '>')); 
    test_sauvegarder_etat(grid, rx, ry); // S2
 
    // Annuler 1 (S2 -> S1)
    assert(test_undo(grid, &rx, &ry)); // Robot en (2,5)  

    // Rétablir 1 (S1 -> S2)
    assert(test_redo(grid, &rx, &ry));
    assert(grid[2][6] == 'O'); // Robot de retour en (2,6) 
    assert(rx == 2 && ry == 6);
    printf("Test redo_simple (1er rétablissement): OK\n");
 
    // Essayer de refaire à nouveau (devrait échouer, plus d'états dans redo)
    assert(!test_redo(grid, &rx, &ry)); 
    printf("Test redo_simple (plus de rétablissement possible): OK\n"); 

    test_liberer_historique();
} 
   
void test_undo_redo() { 
    char grid[MAX_GRID_LINES][MAX_LINE_LENGTH] = {
        "%%%%%%%%%%%%%%%",
        "%-------------%",
        "%---O---------%",  
        "%-------------%",
        "%%%%%%%%%%%%%%%"
    };
    int rows = 5;
    int cols = strlen(grid[0]);
    int rx = 2, ry = 4;

    test_liberer_historique();
    test_sauvegarder_etat(grid, rx, ry); // etat initial (S0)

    // Mouvement 1: S0 -> S1
    assert(simulate_move(grid, rows, cols, &rx, &ry, '>')); 
    test_sauvegarder_etat(grid, rx, ry); // S1

    // mouvement 2: S1 -> S2
    assert(simulate_move(grid, rows, cols, &rx, &ry, '>')); 
    test_sauvegarder_etat(grid, rx, ry); // S2 (etat actuel)
 
    // Annuler: S2 -> S1
    assert(test_undo(grid, &rx, &ry)); 
    assert(rx == 2 && ry == 5);
    printf("Test undo_redo (annulation vers S1): OK\n");

    // Rétablir: S1 -> S2
    assert(test_redo(grid, &rx, &ry));
    assert(rx == 2 && ry == 6);
    printf("Test undo_redo (rétablissement vers S2): OK\n");

    // Annuler: S2 -> S1
    assert(test_undo(grid, &rx, &ry));
    assert(rx == 2 && ry == 5);
    printf("Test undo_redo (annulation vers S1 à nouveau): OK\n");

    // Effectuer un nouveau mouvement depuis S1: S1 -> S3 (l'historique de rétablissement devrait être vidé)
    assert(simulate_move(grid, rows, cols, &rx, &ry, 'v')); // Descendre de (2,5) à (3,5)
    test_sauvegarder_etat(grid, rx, ry); // S3

    assert(rx == 3 && ry == 5);
    printf("Test undo_redo (nouveau mouvement S3): OK\n");

    // Le rétablissement devrait maintenant échouer car l'historique a été vidé par le nouveau mouvement
    assert(!test_redo(grid, &rx, &ry));
    printf("Test undo_redo (rétablissement après un nouveau mouvement, historique vidé): OK\n");

    // Annuler de S3 vers S1
    assert(test_undo(grid, &rx, &ry));
    assert(rx == 2 && ry == 5);
    printf("Test undo_redo (annulation S3 vers S1): OK\n");

    test_liberer_historique();
} 
  
int main() {
    printf("Démarrage des tests...\n");
  
    // Tests de la fonctionnalité 1 (Détection du robot) 
    test_find_robot();
  
    // Tests de la fonctionnalité 2 (Mouvement et poussée) 
    test_move_to_empty();
    test_push_single_box();
    test_push_multiple_boxes();
    test_push_blocked_by_wall(); 
    test_move_into_wall();
    test_move_out_of_bounds();
  
    // Tests de la nouvelle fonctionnalité (Annuler/Rétablir avec Listes Chaînées)  
    test_undo_simple();
    test_redo_simple();
    test_undo_redo(); 
    printf("\nTous les tests ont réussi avec succès !\n"); 
    return 0;
}