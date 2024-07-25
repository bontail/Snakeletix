#ifndef PAINT_H
#define PAINT_H
#include "SDL2/SDL.h"
#include "../../main.h"



int init_paint(PermanentVars const *pv, SDL_Renderer *renderer);
bool check_paint_collision(SDL_Rect const *snakeHeadRect);
void throw_paint(PermanentVars const *pv);
void clear_paint(PermanentVars const *pv);
void render_paint(SDL_Renderer *renderer);
void close_paint();



#endif //PAINT_H
