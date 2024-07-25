#ifndef SNAKE_H
#define SNAKE_H
#include <SDL2/SDL.h>
#include "../main.h"



int init_snake(PermanentVars const *pv, SDL_Renderer *renderer);
void render_snake(SDL_Renderer *renderer);
void set_snake_direction(PermanentVars const *pv, enum Directions direction);
void make_new_snake_frame(PermanentVars const *pv);
void update_length();
void clear_snake(PermanentVars const *pv);
bool can_quit_game();
void close_snake();


#endif //SNAKE_H
