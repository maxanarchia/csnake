#ifndef RENDER_H___
#define RENDER_H___

// Includes

#include <SDL3/SDL.h>
#include "game.h"

// Functions

void render_snake(SDL_Renderer* const renderer, Snake* snake);
void render_food(SDL_Renderer* const renderer, Food* food);
void render_game(SDL_Renderer* const renderer, Game* game);
void* scp(void* pointer);
void scc(int error_code);

#endif
