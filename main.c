// Includes

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "render.h"

// Main

int main() {
    srand(time(0));
    Game game;
    game_init(&game);
    const FoodStats FOOD_DEFS[] = {
        [FOOD_NORMAL] = { .score=+1, .lifetime=3000,  .respawn_delay=1000,    .unique=true  },
        [FOOD_POISON] = { .score=-5, .lifetime=5000,  .respawn_delay=7000, .unique=true  },
        [FOOD_ROTTEN] = { .score=-2, .lifetime=30000, .respawn_delay=0,    .unique=false }
    };

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

                if (key == SDLK_UP && game.snake.direction != DOWN)
                    game.snake.direction = UP;
                else if (key == SDLK_DOWN && game.snake.direction != UP)
                    game.snake.direction = DOWN;
                else if (key == SDLK_LEFT && game.snake.direction != RIGHT)
                    game.snake.direction = LEFT;
                else if (key == SDLK_RIGHT && game.snake.direction != LEFT)
                    game.snake.direction = RIGHT;
            }
        }

        // Update game
        scc(SDL_SetRenderDrawColor(renderer, 25, 25, 35, 255)); // RGBA: pale black
        SDL_RenderClear(renderer);
        render_game(renderer, &game);
        update_game(&game, FOOD_DEFS);
        SDL_RenderPresent(renderer);
    }

    return EXIT_SUCCESS;
}
