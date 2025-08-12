#include "game.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    (void)argc; 
    (void)argv;
    
    Game game;
    
    printf("Initializing Brickout game...\n");
    if (game_init(&game) != 0) {
        fprintf(stderr, "Failed to initialize game\n");
        return 1;
    }
    
    printf("Game initialized successfully. Starting main loop...\n");
    game_run(&game);
    
    printf("Game loop ended. Cleaning up...\n");
    game_cleanup(&game);
    
    printf("Game cleanup complete.\n");
    return 0;
}