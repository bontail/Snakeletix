#ifndef APPLE_H
#define APPLE_H
#include <SDL2/SDL.h>
#include "../../main.h"



int init_apple(SDL_Renderer *renderer);
bool check_apple_collision(SDL_Rect const *snakeHeadRect);
void eat_apple(PermanentVars const *pv);
void clear_apple(PermanentVars const *pv);
void render_apple(SDL_Renderer *renderer);
void close_apple();



#endif //APPLE_H
