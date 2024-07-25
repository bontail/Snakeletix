#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <stdio.h>
#include "utils.h"
#include "main.h"
#include "game/field.h"
#include "game/snake.h"
#include "game/items/apple.h"
#include "game/items/bombs.h"
#include "game/items/portals.h"
#include "game/items/quicksands.h"
#include "game/items/paint.h"
#include "text.h"
#include "menu/menu.h"



int const APPLE_SAMPLES_CHANNEL = 1;
int const BOMB_SAMPLES_CHANNEL = 2;
int const PORTAL_SAMPLES_CHANNEL = 3;
int const MENU_SAMPLES_CHANNEL = 4;
int const GAME_END_SAMPLES_CHANNEL = 5;
int const PAINT_SAMPLES_CHANNEL = 6;


int isHiDPI = 1;


char const *const FOND_FILE = "../staticfiles/fond.png";
SDL_Texture *g_fondTexture = NULL;


SDL_Window *g_window = NULL;
SDL_Renderer *g_renderer = NULL;
PermanentVars g_permanentVars; // variables that will be constant but initialized after the program starts
int g_framesCount = 0;
int g_startTicks;
bool runningApp = true;


enum States {
    MENU,
    GAME,
} state = MENU;

enum GameModes g_gameMode = EASY;



static void set_pv();
int init_libraries();
int init_main_module();
int init_submodules();
int init();
void switch_event_types(SDL_Event);
void render();
void close();



static void set_pv() {
    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);
    g_permanentVars.w = displayMode.w * (1 + isHiDPI);
    g_permanentVars.h = displayMode.h * (1 + isHiDPI);
    g_permanentVars.linesCount = g_permanentVars.h / 100 - 4;
    g_permanentVars.columnsCount = g_permanentVars.w / 100 - 4;
}


int init_libraries() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        printf("SDL_image Error: %s\n", IMG_GetError());
        return 1;
    }

    if (TTF_Init() != 0) {
        printf("TTF_Init Error: %s\n", TTF_GetError());
        return 1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) != 0) {
        printf("SDL_mixer Error: %s\n", Mix_GetError());
        return 1;
    }

    return 0;
}


int init_main_module() {
    g_window = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 0, 0,
                                SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP | (SDL_WINDOW_ALLOW_HIGHDPI * isHiDPI));
    if (g_window == NULL) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        return 1;
    }
    isHiDPI = (int)(SDL_GetWindowFlags(g_window) & (unsigned int)isHiDPI);

    g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_PRESENTVSYNC);
    if (g_renderer == NULL) {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        return 1;
    }

    set_pv();
    int isError = load_texture(g_renderer, FOND_FILE, &g_fondTexture, NULL, NULL, NULL);

    return isError;
}


int init_submodules() {
    if (init_field(&g_permanentVars, g_renderer) != 0) {
        printf("Init background Error\n");
        return 1;
    }

    if (init_snake(&g_permanentVars,g_renderer) != 0) {
        printf("Init snake Error\n");
        return 1;
    }

    if (init_apple(g_renderer) != 0) {
        printf("Init apple Error\n");
        return 1;
    }

    if (init_bombs(g_renderer) != 0) {
        printf("Init bomb Error\n");
        return 1;
    }

    if (init_portals(g_renderer) != 0) {
        printf("Init portal Error\n");
        return 1;
    }

    if (init_quicksands(g_renderer) != 0) {
        printf("Init bomb Error\n");
        return 1;
    }

    if (init_paint(&g_permanentVars, g_renderer) != 0) {
        printf("Init paint Error\n");
        return 1;
    }

    if (init_text(&g_permanentVars) != 0) {
        printf("Init text Error\n");
        return 1;
    }

    if (init_menu(&g_permanentVars, g_renderer) != 0) {
        printf("Init menu Error\n");
        return 1;
    }

    return 0;
}


int init() {
    if (init_libraries() != 0) {
        printf("Init library Error\n");
        return 1;
    }

    if (init_main_module() != 0) {
        printf("Init main module Error\n");
        return 1;
    }

    if (init_submodules() != 0) {
        printf("Init submodules Error\n");
        return 1;
    }


    return 0;
}


void render() {
    SDL_RenderClear(g_renderer);
    SDL_RenderCopy(g_renderer, g_fondTexture, NULL, NULL);
    render_fps_text(g_renderer, g_startTicks);
    if (state == MENU) {
        render_menu(g_renderer);
        render_max_score_text(g_renderer);
    }else if(state == GAME){
        render_field(g_renderer);
        render_quicksands(g_renderer);
        render_apple(g_renderer);
        render_bombs(g_renderer);
        render_snake(g_renderer);
        render_portals(g_renderer);
        render_score_text(g_renderer);
        render_portals_count_text(g_renderer);
        render_paint(g_renderer);
        render_end_game_text(g_renderer);
    }
    SDL_RenderPresent(g_renderer);
}


void close() {
    close_menu();
    close_text();
    close_paint();
    close_quicksands();
    close_portals();
    close_bombs();
    close_apple();
    close_snake();
    close_field();

    SDL_DestroyTexture(g_fondTexture);
    g_fondTexture = NULL;

    SDL_DestroyRenderer(g_renderer);
    g_renderer = NULL;

    SDL_DestroyWindow(g_window);
    g_window = NULL;

    Mix_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}


void check_mouse_collision(){
    SDL_Rect mouseRect = {0, 0, 1, 1};
    SDL_GetMouseState(&mouseRect.x, &mouseRect.y);
    if (isHiDPI) {
        mouseRect.x *= 2;
        mouseRect.y *= 2;
    }
    for (int i = 0; i < ButtonTypes_Count; ++i) {
        if (check_button_collision(&mouseRect, i)){
            press_button(i, &g_gameMode, &runningApp);
            break;
        }
    }
}


void switch_event_types(SDL_Event const event) {
    switch (event.type) {
        case SDL_QUIT:
            runningApp = false;
            break;
        case SDL_MOUSEBUTTONDOWN:
            check_mouse_collision();
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.scancode) {
                case SDL_SCANCODE_W:
                case SDL_SCANCODE_UP:
                    set_snake_direction(&g_permanentVars, UP);
                    break;
                case SDL_SCANCODE_A:
                case SDL_SCANCODE_LEFT:
                    set_snake_direction(&g_permanentVars, LEFT);
                    break;
                case SDL_SCANCODE_S:
                case SDL_SCANCODE_DOWN:
                    set_snake_direction(&g_permanentVars, DOWN);
                    break;
                case SDL_SCANCODE_D:
                case SDL_SCANCODE_RIGHT:
                    set_snake_direction(&g_permanentVars, RIGHT);
                    break;
                default:
                    break;
            }
        default:
            break;
    }
}


int main(void) {
    if (init() != 0) {
        printf("Init Error\n");
        return 1;
    }

    SDL_Event event;
    g_startTicks = (int)SDL_GetTicks();
    clear_menu();
    while (runningApp) {
        render();
        while (SDL_PollEvent(&event)) {
            switch_event_types(event);
        }
        if (state == MENU) {
            if (can_quit_menu()) {
                clear_field();
                clear_snake(&g_permanentVars);
                clear_apple(&g_permanentVars);
                clear_bombs(&g_permanentVars, g_gameMode);
                clear_portals(g_gameMode);
                clear_quicksands(&g_permanentVars, g_gameMode);
                clear_paint(&g_permanentVars);
                clear_text();
                state = GAME;
            }
        }
        else if (state == GAME) {
            make_new_snake_frame(&g_permanentVars);
            if (can_quit_game()) {
                clear_menu();
                state = MENU;
            }
        }
        g_framesCount++;
    }


    close();
    return 0;
}
