#ifndef QUICKSANDS_H
#define QUICKSANDS_H
#include <SDL2/SDL.h>
#include "../../main.h"



int init_quicksands(SDL_Renderer *renderer);
void clear_quicksands(PermanentVars const *pv, enum GameModes gameMode);
void render_quicksands(SDL_Renderer *renderer);
void close_quicksands();



#endif //QUICKSANDS_H
