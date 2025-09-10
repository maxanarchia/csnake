#include "game.h"

void update_snake(Game *game, uint32_t now, FoodStats* FOOD_DEFS) {
    if (now - game->snake.last_spawn_time >= SNAKE_UPDATE_INTERVAL) {
        Cell tail = game->snake.body[game->snake.actual_length - 1];
        for (int i = game->snake.actual_length - 1; i > 0; i--) {
            game->snake.body[i] = game->snake.body[i - 1];
        }
        Cell* head = &game->snake.body[0];
        switch (game->snake.direction) {
            case UP:    head->y -= 1; break;
            case DOWN:  head->y += 1; break;
            case LEFT:  head->x -= 1; break;
            case RIGHT: head->x += 1; break;
        }
        if (check_eat(game, &tail, now, FOOD_DEFS)) {
            game->snake.last_eat_time = now;
        }
        check_starvation(&(game->snake), now);
        check_death(game, now);
        game->snake.last_spawn_time = now;
    }
}

void update_foods(Game* game, uint32_t now, FoodStats* FOOD_DEFS) {
    // Update Normal
    if (game->normal.active && now - game->normal.last_spawn_time >= FOOD_DEFS[FOOD_NORMAL].lifetime) {
        normal_despawn(game, now);
    }
    if (!game->normal.active && now - game->normal.last_spawn_time >= FOOD_DEFS[FOOD_NORMAL].respawn_delay) {
        normal_spawn(game, now);
    }
    // Update Rotten
    for (int i=0; i<game->rotten_count; i++) {
        if (now - game->rottens[i].last_spawn_time >= FOOD_DEFS[FOOD_ROTTEN].lifetime) {
            rotten_despawn(game, i);
        }
    }
    // Update Poison
    if (game->unlock_poison && game->poison.active && now - game->poison.last_spawn_time >= FOOD_DEFS[FOOD_POISON].lifetime) {
        poison_despawn(game, now);
    }
    if (game->unlock_poison && !game->poison.active && now - game->poison.last_spawn_time >= FOOD_DEFS[FOOD_POISON].respawn_delay) {
        poison_spawn(game, now);
    }
}

void update_game(Game* game, FoodStats* FOOD_DEFS) {
    uint32_t now = SDL_GetTicks();
    update_snake(game, now, FOOD_DEFS);
    update_foods(game, now, FOOD_DEFS);
}

void game_init(Game* game) {
    uint32_t now = SDL_GetTicks();
    game->quit = false;
    game->unlock_poison = false;
    game->rotten_count = 0;
    game->poison.active = false;
    game->poison.last_spawn_time = 0;
    snake_spawn(&(game->snake), now);
    normal_spawn(game, now);
}

void normal_spawn(Game* game, uint32_t now) {
    game->normal.cell = random_empty_cell(game);
    game->normal.type = FOOD_NORMAL;
    game->normal.last_spawn_time = now;
    game->normal.active = true;
}

void poison_spawn(Game* game, uint32_t now) {
    game->poison.cell = random_empty_cell(game);
    game->poison.type = FOOD_POISON;
    game->poison.last_spawn_time = now;
    game->poison.active = true;
}

void rotten_spawn(Game* game, uint32_t now) {
    if (game->rotten_count >= MAX_ROTTEN) return;
    game->rottens[game->rotten_count].cell = game->normal.cell;
    game->rottens[game->rotten_count].type = FOOD_ROTTEN;
    game->rottens[game->rotten_count].last_spawn_time = now;
    game->rottens[game->rotten_count].active = true;
    game->rotten_count++;
}

void normal_despawn(Game* game, uint32_t now) {
    rotten_spawn(game, now);
    game->normal.last_spawn_time = now;
    game->normal.active = false;
}

void poison_despawn(Game* game, uint32_t now) {
    game->poison.active = false;
    game->poison.last_spawn_time = now;
}

void rotten_despawn(Game* game, int index) {
    if (index < 0 || index >= game->rotten_count) return;
    for (int i=index; i<game->rotten_count-1; i++) {
        game->rottens[i] = game->rottens[i+1];
    }
    game->rotten_count--;
}

// Returns a random positive integer within [begin,end)
unsigned int random_coordinate(unsigned int begin, unsigned int end) {
    return rand() % (end - begin) + begin;
}

Cell random_cell() {
    Cell cell = {0};
    cell.x = random_coordinate(0, ROWS);
    cell.y = random_coordinate(0, COLS);
    return cell;
}

bool is_cell_free(Game* game, Cell* cell) {
    // Check normal food presence
    if (game->normal.active && cell->x == game->normal.cell.x &&
        cell->y == game->normal.cell.y) {
        return false;
    }
    // Check poison food presence
    if (game->poison.active && cell->x == game->poison.cell.x &&
        cell->y == game->poison.cell.y) {
        return false;
    }
    // Check rotten food presence
    for (int i=0; i<game->rotten_count; i++) {
        if (game->rottens[i].active && cell->x == game->rottens[i].cell.x &&
            cell->y == game->rottens[i].cell.y) {
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

Cell central_cell() {
    Cell center = {0};
    center.x = ROWS / 2;
    center.y = COLS / 2;
    return center;
}

void snake_spawn(Snake* snake, uint32_t now) {
    snake->actual_length = 0;
    snake->body[0] = central_cell();
    snake->actual_length = 1;
    snake->last_spawn_time = now;
    snake->last_eat_time = 0;
    snake->direction = RIGHT;
}

bool check_eat(Game* game, Cell* tail, uint32_t now, FoodStats* FOOD_DEFS) {
    // Normal
    if (game->normal.active && game->snake.body[0].x == game->normal.cell.x &&
        game->snake.body[0].y == game->normal.cell.y) {
        game->snake.actual_length += FOOD_DEFS[FOOD_NORMAL].score;
        game->snake.body[game->snake.actual_length - 1] = *tail;
        if (game->snake.actual_length < MAX_SNAKE_LENGTH) {
            normal_spawn(game, now);
            if (!game->unlock_poison && game->snake.actual_length == 6) {
                // Init Poison Food
                game->unlock_poison = true;
                poison_spawn(game, now);
            }
        } else {
            game->quit = true;
        }
        return true;
    }
    // Poison
    if (game->poison.active && game->snake.body[0].x == game->poison.cell.x &&
        game->snake.body[0].y == game->poison.cell.y) {
        game->snake.actual_length += FOOD_DEFS[FOOD_POISON].score;
        poison_despawn(game, now);
        return true;
    }
    // Rottens
    for (int i=0; i<game->rotten_count; i++) {
        if (game->rottens[i].active && game->snake.body[0].x == game->rottens[i].cell.x &&
            game->snake.body[0].y == game->rottens[i].cell.y) {
            game->snake.actual_length += FOOD_DEFS[FOOD_ROTTEN].score;
            rotten_despawn(game, i);
            return true;
        }
    }
    return false;
}

void check_death(Game* game, uint32_t now) {
    // Death by starvation
    if (game->snake.actual_length <= 0) {
        game->quit = true;
        return;
    }
    // Death by escaping the borders
    if (game->snake.body[0].x < 0 || game->snake.body[0].x >= ROWS ||
        game->snake.body[0].y < 0 || game->snake.body[0].y >= COLS) {
        game->quit = true;
        return;
    }
    // Death by touching its own body
    for (int i=1; i<game->snake.actual_length; i++) {
        if (game->snake.body[0].x == game->snake.body[i].x &&
            game->snake.body[0].y  == game->snake.body[i].y) {
            game->quit = true;
            return;
        }
    }
}

void check_starvation(Snake* snake, uint32_t now) {
    if (now - snake->last_eat_time >= STARVATION_INTERVAL) {
        snake->actual_length--;
        snake->last_eat_time = now;
    }
}
