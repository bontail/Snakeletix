#include <SDL2/SDL.h>
#include <SDL_mixer.h>
#include <limits.h>
#include "../utils.h"
#include "menu.h"



extern int g_framesCount;


static int const MENU_PAUSED_FRAMES_COUNT = 15; // Frames count after click the button and before start the game
static int FRAMES_END_MENU_PAUSE;


static int const MAX_BUTTON_INCREASE = 100;
static char const *const START_BUTTON_TEXTURE_FILES[] = {
        "../staticfiles/menu/unpressedStartButton.png",
        "../staticfiles/menu/pressedStartButton.png"
};
#define START_BUTTON_TEXTURES_COUNT sizeof(START_BUTTON_TEXTURE_FILES) / sizeof(char const *)
static SDL_Texture *g_startButtonTextures[START_BUTTON_TEXTURES_COUNT];
static struct StartButton {
    SDL_Rect rect;
    bool isPressed;
} g_startButton;


static char const *const DIFFICULTY_BUTTON_TEXTURE_FILES[GameModes_Count] = {
        "../staticfiles/menu/easyButton.png",
        "../staticfiles/menu/normalButton.png",
        "../staticfiles/menu/hardButton.png"
};
static struct DifficultyButton {
    SDL_Texture *texture;
    SDL_Rect rect;
    int alphaValue;
} g_difficultyButtons[GameModes_Count];


static char const *const QUIT_BUTTON_TEXTURE_FILE = "../staticfiles/menu/quitButton.png";
static SDL_Texture *g_quitButtonTexture = NULL;
static struct QuitButton{
    SDL_Rect rect;
} g_quitButton;


static char const *const MENU_MUSIC_FILE = "../staticfiles/menu/music.mp3";
static Mix_Music *g_menuMusic = NULL;


static char const *const CLICK_BUTTON_SAMPLE_FILE = "../staticfiles/menu/clickButtonSample.wav";
static Mix_Chunk *g_clickButtonSample = NULL;
extern int const MENU_SAMPLES_CHANNEL;



static int load_menu_media(SDL_Renderer *renderer) {
    int isError = 0;

    isError |= load_textures(renderer, START_BUTTON_TEXTURES_COUNT, START_BUTTON_TEXTURE_FILES, g_startButtonTextures, NULL);
    SDL_QueryTexture(g_startButtonTextures[0], NULL, NULL, &(g_startButton.rect.w), &(g_startButton.rect.h));

    for (int i = 0; i < GameModes_Count; ++i) {
        isError |= load_texture(renderer, DIFFICULTY_BUTTON_TEXTURE_FILES[i], &g_difficultyButtons[i].texture, &(g_difficultyButtons[i].rect.w), &(g_difficultyButtons[i].rect.h), NULL);
    }

    isError |= load_texture(renderer, QUIT_BUTTON_TEXTURE_FILE, &g_quitButtonTexture, &g_quitButton.rect.w, &g_quitButton.rect.h, NULL);

    isError |= load_music(MENU_MUSIC_FILE, &g_menuMusic);
    isError |= load_sample(CLICK_BUTTON_SAMPLE_FILE, &g_clickButtonSample);

    return isError;
}


static void clear_alpha_values() {
    for (int i = 0; i < GameModes_Count; ++i) {
        g_difficultyButtons[i].alphaValue = 100;
    }
}


static void set_buttons_alpha_mod() {
    for (int i = 0; i < GameModes_Count; ++i) {
        SDL_SetTextureAlphaMod(g_difficultyButtons[i].texture, g_difficultyButtons[i].alphaValue);
    }
}


int init_menu(PermanentVars const *pv, SDL_Renderer *renderer) {
    if (load_menu_media(renderer) != 0) {
        return 1;
    }
    g_startButton.rect.x = pv->w / 2;
    g_startButton.rect.y = pv->h / 2;

    g_quitButton.rect.x = pv->w - g_quitButton.rect.w;

    for (int i = 0; i < GameModes_Count; ++i) {
        g_difficultyButtons[i].rect.x = pv->w / 20;
        g_difficultyButtons[i].rect.y = (pv->h / 10) * (10 - GameModes_Count + i);
    }

    clear_alpha_values();
    g_difficultyButtons[EASY].alphaValue = 255;
    set_buttons_alpha_mod();

    return 0;

}


static void update_paint_rect(SDL_Rect *rect){
    update_rect(rect, MAX_BUTTON_INCREASE, true, 3);
}


bool check_button_collision(SDL_Rect const *mouseRect, enum ButtonTypes ButtonTypes) {
    SDL_Rect buttonRect;
    switch (ButtonTypes) {
        case START_BUTTON:
            buttonRect = g_startButton.rect;
            update_paint_rect(&buttonRect);
            break;
        case QUIT_BUTTON:
            buttonRect = g_quitButton.rect;
            break;
        case EASY_BUTTON:
            buttonRect = g_difficultyButtons[EASY].rect;
            break;
        case NORMAL_BUTTON:
            buttonRect = g_difficultyButtons[NORMAL].rect;
            break;
        case HARD_BUTTON:
            buttonRect = g_difficultyButtons[HARD].rect;
            break;
        default:
            printf("Illegal button type");
            exit(1);
    }
    return SDL_HasIntersection(mouseRect, &buttonRect);
}


void press_button(enum ButtonTypes ButtonTypes, enum GameModes *gameMode, bool *runningApp) {
    switch (ButtonTypes) {
        case START_BUTTON:
            g_startButton.isPressed = true;
            FRAMES_END_MENU_PAUSE = g_framesCount + MENU_PAUSED_FRAMES_COUNT;
            break;
        case QUIT_BUTTON:
            *runningApp = false;
            break;
        case EASY_BUTTON:
            *gameMode = EASY;
            break;
        case NORMAL_BUTTON:
            *gameMode = NORMAL;
            break;
        case HARD_BUTTON:
            *gameMode = HARD;
            break;
        default:
            printf("Illegal button type");
            exit(1);
    }

    clear_alpha_values();
    g_difficultyButtons[*gameMode].alphaValue = 255;
    set_buttons_alpha_mod();

    Mix_PlayChannel(MENU_SAMPLES_CHANNEL, g_clickButtonSample, 0);
}


bool can_quit_menu() {
    return FRAMES_END_MENU_PAUSE < g_framesCount;
}


void clear_menu() {
    g_startButton.isPressed = false;
    FRAMES_END_MENU_PAUSE = INT_MAX;
    Mix_HaltMusic();
    Mix_PlayMusic(g_menuMusic, -1);
}


void render_menu(SDL_Renderer *renderer) {
    SDL_Rect startButtonRect = g_startButton.rect;
    update_paint_rect(&startButtonRect);
    SDL_RenderCopy(renderer, g_startButtonTextures[g_startButton.isPressed], NULL, &startButtonRect);
    for (int i = 0; i < GameModes_Count; ++i) {
        SDL_RenderCopy(renderer, g_difficultyButtons[i].texture, NULL, &g_difficultyButtons[i].rect);
    }

    SDL_RenderCopy(renderer, g_quitButtonTexture, NULL, &g_quitButton.rect);
}


void close_menu() {
    close_textures(START_BUTTON_TEXTURES_COUNT, g_startButtonTextures);

    for (int i = 0; i < GameModes_Count; ++i) {
        close_texture(&g_difficultyButtons[i].texture);
    }

    close_texture(&g_quitButtonTexture);

    close_music(&g_menuMusic);
    close_sample(&g_clickButtonSample);
}