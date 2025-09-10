#include "render.h"

void render_snake(SDL_Renderer* const renderer, Snake* snake) {
    scc(SDL_SetRenderDrawColor(renderer, 50, 205, 50, 255)); // RGBA: green accent
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

void render_food(SDL_Renderer* const renderer, Food* food) {
    switch (food->type) {
        case FOOD_NORMAL:
            scc(SDL_SetRenderDrawColor(renderer, 255, 70, 70, 255)); // RGBA: apple pink
            break;
        case FOOD_ROTTEN:
            scc(SDL_SetRenderDrawColor(renderer, 139, 110, 70, 255)); // RGBA: opaque brown
            break;
        case FOOD_POISON:
            scc(SDL_SetRenderDrawColor(renderer, 180, 0, 255, 255)); // RGBA: electric purple
            break;
        default:
            scc(SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0)); // RGBA: white
            break;
    }
    if (food->active) {
        SDL_FRect const rectangle = {
            food->cell.x * CELL_WIDTH,
            food->cell.y * CELL_HEIGHT,
            CELL_WIDTH,
            CELL_HEIGHT
        };
        scc(SDL_RenderFillRect(renderer, &rectangle));
    }
}

void render_game(SDL_Renderer* const renderer, Game* game) {
    render_snake(renderer, &(game->snake));
    render_food(renderer, &(game->normal));
    render_food(renderer, &(game->poison));
    for (int i=0; i<game->rotten_count; i++) {
        render_food(renderer, (game->rottens) + i);
    }
}

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
