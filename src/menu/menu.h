#ifndef MENU_H
#define MENU_H
#include <SDL2/SDL.h>
#include "../main.h"



enum ButtonTypes{
    START_BUTTON,
    QUIT_BUTTON,
    EASY_BUTTON,
    NORMAL_BUTTON,
    HARD_BUTTON,
    ButtonTypes_Count,
};


int init_menu(PermanentVars const *pv, SDL_Renderer *renderer);
bool check_button_collision(SDL_Rect const *mouseRect, enum ButtonTypes ButtonTypes);
void press_button(enum ButtonTypes ButtonTypes, enum GameModes *gameMode, bool *runningApp);
bool can_quit_menu();
void clear_menu();
void render_menu(SDL_Renderer *renderer);
void close_menu();



#endif //MENU_H
