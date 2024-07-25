#include <SDL2/SDL.h>
#include "../../utils.h"
#include "../field.h"



#define MAX_BOMBS_COUNT 12
static int const MAX_BOMB_REDUCTION = 10;
static char const *const BOMB_TEXTURE_FILE = "../staticfiles/items/bombs/bomb.png";
static SDL_Texture *g_bombTexture;
static struct Bomb{
    SDL_Rect rect;
} g_bombs[MAX_BOMBS_COUNT];
int g_bombsCount;


static char const *const BOMB_DETONATION_SAMPLE_FILE = "../staticfiles/items/bombs/bombDetonationSample.wav";
static Mix_Chunk *g_bombDetonationSample = NULL;
extern int const BOMB_SAMPLES_CHANNEL;



int init_bombs(SDL_Renderer *renderer) {
    int isError = 0;

    isError |= load_texture(renderer, BOMB_TEXTURE_FILE, &g_bombTexture, &(g_bombs[0].rect.w), &(g_bombs[0].rect.h), NULL);
    fill_rect_wh(g_bombs[0].rect.w, g_bombs[0].rect.h, g_bombs, sizeof(struct Bomb), MAX_BOMBS_COUNT);
    isError |= load_sample(BOMB_DETONATION_SAMPLE_FILE, &g_bombDetonationSample);

    return isError;
}


static void update_bomb_rect(SDL_Rect *rect, int index){
    update_rect(rect, MAX_BOMB_REDUCTION + index * 3, false, 1);
}


bool check_bomb_collision(SDL_Rect const *snakeHeadRect) {
    for (int i = 0; i < g_bombsCount; ++i) {
        SDL_Rect bombRect = g_bombs[i].rect;
        update_bomb_rect(&bombRect, i);
        if (SDL_HasIntersection(&bombRect, snakeHeadRect)){
            return true;
        }
    }
    return false;
}


void detonate_bomb(){
    Mix_PlayChannel(BOMB_SAMPLES_CHANNEL, g_bombDetonationSample, 0);
}


void clear_bombs(PermanentVars const *pv, enum GameModes gameMode) {
    g_bombsCount = MAX_BOMBS_COUNT / (int)(GameModes_Count - gameMode);
    int bombLine, bombColumn;
    for (int i = 0; i < g_bombsCount; ++i) {
        set_random_free_cell(&bombLine, &bombColumn, true);
        get_cell(bombLine, bombColumn)->isFree = false;
        set_position(pv, &(g_bombs[i].rect), (float)bombLine + 0.5f, (float)bombColumn + 0.5f);
    }
}


void render_bombs(SDL_Renderer *renderer) {
    for (int i = 0; i < g_bombsCount; ++i) {
        SDL_Rect bombRect = g_bombs[i].rect;
        update_bomb_rect(&bombRect, i);
        SDL_RenderCopy(renderer, g_bombTexture, NULL, &bombRect);
    }
}


void close_bombs() {
    close_texture(&g_bombTexture);
    close_sample(&g_bombDetonationSample);
}
