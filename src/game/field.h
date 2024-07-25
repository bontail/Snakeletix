#ifndef FIELD_H
#define FIELD_H
#include <SDL2/SDL.h>
#include "../main.h"



typedef struct Cell {
    enum Directions direction;
    bool isFree;
} Cell;


int init_field(PermanentVars *pv, SDL_Renderer *renderer);
Cell *get_cell(int line , int column);
void set_random_free_cell(int *line, int *column, bool excludeFirstLineLine);
void clear_field();
void render_field(SDL_Renderer *renderer);
void close_field();



#endif //FIELD_H
