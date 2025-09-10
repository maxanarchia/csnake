#ifndef GAME_H___
#define GAME_H___

// Includes

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL3/SDL.h>

// Definitions

#define ROWS 20
#define COLS 20
#define MAX_ROTTEN ((ROWS) * (COLS))
#define SCREEN_WIDTH 500
#define SCREEN_HEIGHT 500
#define FOOD_UPDATE_INTERVAL 3000 // 3   seconds
#define SNAKE_UPDATE_INTERVAL 100 // 0,2 seconds
#define STARVATION_INTERVAL 5000  // 5   seconds
#define MAX_SNAKE_LENGTH 100
#define CELL_WIDTH ((SCREEN_WIDTH / ROWS))
#define CELL_HEIGHT ((SCREEN_HEIGHT / COLS))

// Data Types

typedef struct {
    int x; // 0 = left; SCREEN_WIDTH = right
    int y; // 0 = up; SCREEN_HEIGHT = down
} Cell;

typedef enum { FOOD_NORMAL, FOOD_POISON, FOOD_ROTTEN } FoodType;

typedef struct {
    int score;              // +1, -5, -2
    uint32_t lifetime;      // 3000, 5000, 30000
    uint32_t respawn_delay; // 1000, 7000, 0
    bool unique;            // true for NORMAL and POISON, false for ROTTEN
} FoodStats;

typedef struct {
    Cell cell;
    FoodType type; // NORMAL, POISON, ROTTEN
    uint32_t last_spawn_time;
    bool active;
} Food;

typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction;

typedef struct {
    Cell body[MAX_SNAKE_LENGTH];
    int actual_length;
    Direction direction;
    uint32_t last_spawn_time;
    uint32_t last_eat_time;
} Snake;

typedef struct {
    bool quit;
    bool unlock_poison;
    Snake snake;
    Food normal;  
    Food poison;
    Food rottens[MAX_ROTTEN];
    int rotten_count;
} Game;

// Functions

void update_snake(Game *game, uint32_t now, FoodStats* FOOD_DEFS);
void update_foods(Game* game, uint32_t now, FoodStats* FOOD_DEFS);
void update_game(Game* game, FoodStats* FOOD_DEFS);
void game_init(Game* game);
void normal_spawn(Game* game, uint32_t now);
void poison_spawn(Game* game, uint32_t now);
void rotten_spawn(Game* game, uint32_t now);
void normal_despawn(Game* game, uint32_t now);
void poison_despawn(Game* game, uint32_t now);
void rotten_despawn(Game* game, int index);
unsigned int random_coordinate(unsigned int begin, unsigned int end);
Cell random_cell();
bool is_cell_free(Game* game, Cell* cell);
Cell random_empty_cell(Game* game);
Cell central_cell();
void snake_spawn(Snake* snake, uint32_t now);
bool check_eat(Game* game, Cell* tail, uint32_t now, FoodStats* FOOD_DEFS);
void check_death(Game* game, uint32_t now);
void check_starvation(Snake* snake, uint32_t now);

#endif
