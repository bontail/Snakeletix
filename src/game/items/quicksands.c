#include <SDL2/SDL.h>
#include "../../utils.h"
#include "../field.h"



static char const *const QUICKSAND_TEXTURE_FILE = "../staticfiles/items/quicksands/quicksand.png";
static SDL_Texture *g_quicksandTexture = NULL;
#define MAX_QUICKSANDS_COUNT 12
static struct Quicksand{
    SDL_Rect rect;
} g_quicksands[MAX_QUICKSANDS_COUNT];
static int g_quicksandsCount;



int init_quicksands(SDL_Renderer *renderer) {
    int isError = 0;

    isError |= load_texture(renderer, QUICKSAND_TEXTURE_FILE, &g_quicksandTexture, &(g_quicksands[0].rect.w), &(g_quicksands[0].rect.h), NULL);
    fill_rect_wh(g_quicksands[0].rect.w, g_quicksands[0].rect.h, g_quicksands, sizeof(struct Quicksand), MAX_QUICKSANDS_COUNT);

    return isError;
}


void clear_quicksands(PermanentVars const *pv, enum GameModes gameMode) {
    g_quicksandsCount = MAX_QUICKSANDS_COUNT / (int)(GameModes_Count - gameMode);
    int quicksandLine, quicksandColumn;
    for (int i = 0; i < g_quicksandsCount; ++i) {
        set_random_free_cell(&quicksandLine, &quicksandColumn, true);
        get_cell(quicksandLine, quicksandColumn)->direction = CLOSE_DIRECTION;
        set_position(pv, &(g_quicksands[i].rect), (float)quicksandLine, (float)quicksandColumn);
    }
}


void render_quicksands(SDL_Renderer *renderer) {
    for (int i = 0; i < g_quicksandsCount; ++i) {
        SDL_RenderCopy(renderer, g_quicksandTexture, NULL, &g_quicksands[i].rect);
    }
}


void close_quicksands(){
    close_texture(&g_quicksandTexture);
}