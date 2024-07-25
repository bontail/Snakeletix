#include <SDL2/SDL.h>
#include "../../utils.h"
#include "../field.h"



static int const MAX_APPLE_REDUCTION = 30;
static char const *const APPLE_TEXTURE_FILES = "../staticfiles/items/apple/apple.png";
static SDL_Texture *g_appleTexture = NULL;
static struct Apple{
    SDL_Rect rect;
} g_apple;


static char const *const EATING_APPLE_SAMPLE_FILE = "../staticfiles/items/apple/eatingAppleSample.wav";
static Mix_Chunk *g_eatingAppleSample = NULL;
extern int const APPLE_SAMPLES_CHANNEL;



int init_apple(SDL_Renderer *renderer) {
    int isError = 0;

    isError |= load_texture(renderer, APPLE_TEXTURE_FILES, &g_appleTexture, &(g_apple.rect.w), &(g_apple.rect.h), NULL);
    isError |= load_sample(EATING_APPLE_SAMPLE_FILE, &g_eatingAppleSample);

    return isError;
}


static void update_apple_rect(SDL_Rect *rect){
    update_rect(rect, MAX_APPLE_REDUCTION, false, 2);
}


bool check_apple_collision(SDL_Rect const *snakeHeadRect) {
    SDL_Rect appleRect = g_apple.rect;
    update_apple_rect(&appleRect);
    return SDL_HasIntersection(&appleRect, snakeHeadRect);
}


static void set_random_apple_position(PermanentVars const *pv, bool excludeFirstLineLine) {
    int appleLine, appleColumn;
    set_random_free_cell(&appleLine, &appleColumn, excludeFirstLineLine);
    get_cell(appleLine, appleColumn)->isFree = false;
    set_position(pv, &(g_apple.rect), (float)appleLine + 0.5f, (float)appleColumn + 0.5f);
}


void eat_apple(PermanentVars const *pv) {
    Mix_PlayChannel(APPLE_SAMPLES_CHANNEL, g_eatingAppleSample, 0);
    set_random_apple_position(pv, false);
}


void clear_apple(PermanentVars const *pv) {
    set_random_apple_position(pv, true);
}


void render_apple(SDL_Renderer *renderer) {
    SDL_Rect appleRect = g_apple.rect;
    update_apple_rect(&appleRect);
    SDL_RenderCopy(renderer, g_appleTexture, NULL, &appleRect);
}


void close_apple() {
    close_texture(&g_appleTexture);
    close_sample(&g_eatingAppleSample);
}