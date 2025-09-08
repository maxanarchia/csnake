#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL3/SDL.h>

// Screen Parameters

#define SCREEN_WIDTH 500
#define SCREEN_HEIGHT 500
#define ROWS 20
#define COLS 20
#define CELL_WIDTH ((SCREEN_WIDTH / ROWS))
#define CELL_HEIGHT ((SCREEN_HEIGHT / COLS))

// Game Parameters

#define FOOD_SPAWN_DELAY 3 // Seconds
#define FOOD_SPAWN_QUANTITY 1

// Data Structures

typedef struct {
    unsigned float x; // 0 = Ovest; SCREEN_WIDTH = West
    unsigned float y; // 0 = North; SCREEN_HEIGHT = South
} Position;

typedef struct {
    Position position;
    unsigned int score;
} Food;

typedef struct {
    bool quit;
    Food food[FOOD_SPAWN_QUANTITY];
} Game;


// Error Wrappers

void* scp(void* pointer) {
    if (!pointer) {
        printf("SDL error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    return pointer;
}

void scc(int error_code) {
    if (error_code < 0) {
        printf("SDL error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
}

// Graphic Renderers

void render_board(SDL_Renderer* const renderer) {
    scc(SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0)); // RGBA
    for (int row=0; row<=ROWS; row++) {
        SDL_RenderLine(renderer, 0, row*CELL_HEIGHT, SCREEN_WIDTH, row*CELL_HEIGHT);
    }
    for (int col=0; col<=COLS; col++) {
        SDL_RenderLine(renderer, col*CELL_HEIGHT, 0, col*CELL_WIDTH, SCREEN_HEIGHT);
    }
}

// Game Management

int random_coordinate(unsigned int begin, unsigned int end) {

}

Position random_position() {
    Position position = {0};
    position.x = random_coordinate(0, ROWS);
    position.y = random_coordinate(0, COLS);
}

bool is_position_empty(Game* game, Position* position) {
    // Check Food presence
    for (int i=0; i<FOOD_SPAWN_QUANTITY; i++) {
        if (position->x == game->)
    }

    // Check Obstacle presence

    // Check Snake presence
}

Position random_empty_position(Game* game) {
    Position position = {0}; // Empty Position

    // Generate random positions until one is empty
    do
    {
        position = random_position();
    } while (is_position_empty(game, &position));

    return position;
    
}

void food_init(Game* game) {
    for (int i=0; i<FOOD_SPAWN_QUANTITY; i++) {
        game->food[i].score = 1;
        game->food[i].position = random_empty_position(&game);
    }
}

void game_init(Game* game) {
    game->quit = false;
    food_init(&game);
}

// Main

int main() {
    Game game;
    game_init(&game);

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* const window = scp(SDL_CreateWindow("Description", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE));
    SDL_Renderer* const renderer = scp(SDL_CreateRenderer(window, NULL));

    while (!game.quit) {
        SDL_Event event;

        // Event extraction
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                game.quit = 1;
            }
        }

        // Update game
        scc(SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0)); // RGBA
        SDL_RenderClear(renderer);
        render_board(renderer);
        SDL_RenderPresent(renderer);
    }

    return EXIT_SUCCESS;
}
