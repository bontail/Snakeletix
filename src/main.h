#ifndef MAIN_H
#define MAIN_H
#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>



#define MAX(a,b)             \
({                           \
__typeof__ (a) _a = (a); \
__typeof__ (b) _b = (b); \
_a > _b ? _a : _b;       \
})


#define MIN(a,b)             \
({                           \
__typeof__ (a) _a = (a); \
__typeof__ (b) _b = (b); \
_a < _b ? _a : _b;       \
})


enum Directions {
    RIGHT,
    DOWN,
    LEFT,
    UP,
    NULL_DIRECTION,
    CLOSE_DIRECTION,
    Directions_COUNT,
};


enum GameModes {
    EASY,
    NORMAL,
    HARD,
    GameModes_Count
};


typedef struct PermanentVars {
    int cellEdge,
    w, h,
    wIndentation, hIndentation,
    wArea, hArea,
    linesCount, columnsCount;
} PermanentVars;



#endif //MAIN_H
