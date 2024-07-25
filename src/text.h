#ifndef TEXT_H
#define TEXT_H
#include <SDL2/SDL.h>
#include "main.h"



int init_text(PermanentVars const *pv);
void render_fps_text(SDL_Renderer *renderer, int startTicks);
void render_score_text(SDL_Renderer *renderer);
void render_portals_count_text(SDL_Renderer *renderer);
void render_max_score_text(SDL_Renderer *renderer);
void set_render_end_game_text_function(bool isWin);
void render_end_game_text(SDL_Renderer *renderer);
void clear_text();
void close_text();



#endif //TEXT_H

