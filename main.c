#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <SDL3/SDL.h>

// Screen Parameters

#define SCREEN_WIDTH 500
#define SCREEN_HEIGHT 500
#define ROWS 20
#define COLS 20
#define CELL_WIDTH ((SCREEN_WIDTH / ROWS))
#define CELL_HEIGHT ((SCREEN_HEIGHT / COLS))
#define MAX_SNAKE_LENGTH ((ROWS) * (COLS))
#define FOOD_UPDATE_INTERVAL 3000    // 3 secondi
#define SNAKE_UPDATE_INTERVAL 100    // 0,2 secondi

// Game Parameters

#define FOOD_SPAWN_DELAY 3 // Seconds
#define FOOD_SPAWN_RATE 1

// Data Structures

typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction;

typedef struct {
    unsigned int x; // 0 = left; SCREEN_WIDTH = right
    unsigned int y; // 0 = up; SCREEN_HEIGHT = down
} Cell;

typedef struct {
    Cell cell;
    unsigned int score;
} Food;

typedef struct {
    Cell body[MAX_SNAKE_LENGTH];
    unsigned int actual_length;
} Snake;

typedef struct {
    bool quit;
    Food food[FOOD_SPAWN_RATE];
    Snake snake;
    Uint32 last_food_update;
    Uint32 last_snake_update;
    Direction snake_direction;
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

void render_snake(SDL_Renderer* const renderer, Game* game) {
    scc(SDL_SetRenderDrawColor(renderer, 255, 222, 33, 0)); // RGBA:
    Snake* snake = &(game->snake);
    for (int i=0; i<snake->actual_length; i++) {
        SDL_FRect const rectangle = {
            snake->body[i].x * CELL_WIDTH,
            snake->body[i].y * CELL_HEIGHT,
            CELL_WIDTH,
            CELL_HEIGHT
        };
        scc(SDL_RenderFillRect(renderer, &rectangle));
    }
}

void render_food(SDL_Renderer* const renderer, Game* game) {
    scc(SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0)); // RGBA: white
    for (int i=0; i<FOOD_SPAWN_RATE; i++) {
        Food* food = &(game->food[i]);
        if (food->score > 0) {
            SDL_FRect const rectangle = {
                food->cell.x * CELL_WIDTH,
                food->cell.y * CELL_HEIGHT,
                CELL_WIDTH,
                CELL_HEIGHT
            };
            scc(SDL_RenderFillRect(renderer, &rectangle));
        }
    }

}

void render_board(SDL_Renderer* const renderer) {
    scc(SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0)); // RGBA: white
    for (int row=0; row<=ROWS; row++) {
        SDL_RenderLine(renderer, 0, row*CELL_HEIGHT, SCREEN_WIDTH, row*CELL_HEIGHT);
    }
    for (int col=0; col<=COLS; col++) {
        SDL_RenderLine(renderer, col*CELL_HEIGHT, 0, col*CELL_WIDTH, SCREEN_HEIGHT);
    }
}

void render_game(SDL_Renderer* const renderer, Game* game) {
    render_board(renderer);
    render_snake(renderer, game);
    render_food(renderer, game);
}

// Game Management

// Returns a random positive integer within [begin,end)
unsigned int random_coordinate(unsigned int begin, unsigned int end) {
    return rand() % (end - begin) + begin + 1;
}

Cell random_cell() {
    Cell cell = {0};
    cell.x = random_coordinate(0, ROWS);
    cell.y = random_coordinate(0, COLS);
    return cell;
}

bool is_cell_free(Game* game, Cell* cell) {
    // Check Food presence
    for (int i=0; i<FOOD_SPAWN_RATE; i++) {
        if (cell->x == game->food[i].cell.x &&
            cell->y == game->food[i].cell.y) {
            return false;
        }
    }

    // Check Obstacle presence

    // Check Snake presence
    for (int i=0; i<game->snake.actual_length; i++) {
        if (cell->x == game->snake.body[i].x &&
            cell->y == game->snake.body[i].y) {
            return false;
        }
    }

    return true;
}

Cell random_empty_cell(Game* game) {
    Cell cell = {0}; // Empty Cell

    // Generate random cells until one is empty
    do
    {
        cell = random_cell();
    } while (!is_cell_free(game, &cell));

    return cell;
}

// void eat_check(Game* game) {
//     for (int i=0; i<FOOD_SPAWN_RATE; i++) {
//         Snake* snake = game->snake;
//         Food* food = game->food[i];
//         if (snake->body[0].x == food->cell.x &&
//             snake->body[0].y == food->cell.y) {
//             return;
//         }
//     }
//     return;
// }

void update_snake(Game *game, Uint32* now) {
    if (*now - game->last_snake_update >= SNAKE_UPDATE_INTERVAL) {
        for (int i = game->snake.actual_length - 1; i > 0; i--) {
            game->snake.body[i] = game->snake.body[i - 1];
        }
        Cell* head = &game->snake.body[0];
        switch (game->snake_direction) {
            case UP:    if (head->y > 0) head->y -= 1; break;
            case DOWN:  if (head->y + 1 < COLS) head->y += 1; break;
            case LEFT:  if (head->x > 0) head->x -= 1; break;
            case RIGHT: if (head->x + 1 < ROWS) head->x += 1; break;
        }
        game->last_snake_update = *now;
    }
    // eat_check(Game* game);
}

void update_food(Game* game, Uint32* now) {
    if (*now - game->last_food_update >= FOOD_UPDATE_INTERVAL) {
        for (int i = 0; i < FOOD_SPAWN_RATE; i++) {
            game->food[i].cell = random_empty_cell(game);
        }
        game->last_food_update = *now;
    }
}

void update_game(Game* game) {
    Uint32 now = SDL_GetTicks();
    update_snake(game, &now);
    update_food(game, &now);
}

void snake_init(Game* game) {
    game->snake.actual_length = 0;
    game->snake.body[0] = random_empty_cell(game);
    game->snake.actual_length = 1;
}

void food_init(Game* game) {
    for (int i=0; i<FOOD_SPAWN_RATE; i++) {
        game->food[i].score = 1;
        game->food[i].cell = random_empty_cell(game);
    }
}

void game_init(Game* game) {
    game->last_snake_update = SDL_GetTicks();
    game->last_food_update = SDL_GetTicks();
    game->quit = false;
    game->snake_direction = RIGHT;
    snake_init(game);
    food_init(game);
}

// Main

int main() {
    srand(time(0));
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
                game.quit = true;
            } else if (event.type == SDL_EVENT_KEY_DOWN) {
                SDL_Keycode key = event.key.key;

                if (key == SDLK_UP && game.snake_direction != DOWN)
                    game.snake_direction = UP;
                else if (key == SDLK_DOWN && game.snake_direction != UP)
                    game.snake_direction = DOWN;
                else if (key == SDLK_LEFT && game.snake_direction != RIGHT)
                    game.snake_direction = LEFT;
                else if (key == SDLK_RIGHT && game.snake_direction != LEFT)
                    game.snake_direction = RIGHT;
            }
        }

        // Update game
        scc(SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0)); // RGBA
        SDL_RenderClear(renderer);
        render_game(renderer, &game);
        update_game(&game);
        SDL_RenderPresent(renderer);
    }

    return EXIT_SUCCESS;
}
