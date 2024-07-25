#ifndef BOMBS_H
#define BOMBS_H
#include <SDL2/SDL.h>
#include "../../main.h"



int init_bombs(SDL_Renderer *renderer);
bool check_bomb_collision(SDL_Rect const *snakeHeadRect);
void detonate_bomb();
void clear_bombs(PermanentVars const *pv, enum GameModes gameMode);
void render_bombs(SDL_Renderer *renderer);
void close_bombs();



#endif //BOMBS_H
