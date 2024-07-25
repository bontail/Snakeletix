#ifndef PORTAL_H
#define PORTAL_H
#include <SDL2/SDL.h>
#include "../../main.h"



int init_portals(SDL_Renderer *renderer);
void clear_portals(enum GameModes gameMode);
bool can_create_portal();
void create_portal(int x, int y, enum Directions direction);
void delete_portal(int x, int y);
void render_portals(SDL_Renderer *renderer);
void close_portals();



#endif //PORTAL_H
