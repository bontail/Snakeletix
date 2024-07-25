#include <SDL2/SDL.h>
#include "../../utils.h"
#include "../field.h"



extern int g_framesCount;


static int const MAX_PAINT_REDUCTION = 13;
static char const *const PAINT_TEXTURE_FILE = "../staticfiles/items/paint/paint.png";
static SDL_Texture *g_paintTexture;
static struct Paint{
    SDL_Rect rect;
} g_paint;


static char const *const PAINTED_DISPLAY_TEXTURE_FILES[] = {"../staticfiles/items/paint/paintedDisplay1.png",
                                             "../staticfiles/items/paint/paintedDisplay2.png",
};
#define PAINTED_DISPLAY_FILES_COUNT (int)(sizeof(PAINTED_DISPLAY_TEXTURE_FILES) / sizeof(char const *))
static SDL_Texture *g_paintedDisplayTextures[PAINTED_DISPLAY_FILES_COUNT];
static struct PaintedDisplay{
    SDL_Rect rect;
    int textureNumber;
} g_paintedDisplay;
static int g_startPaintedDisplay;
static int const PAINTED_DISPLAY_DELAY = 2;


static char const *const PAINT_THROW_SAMPLE_FILE = "../staticfiles/items/paint/throwPaintSample.wav";
static Mix_Chunk *g_paintThrowSample = NULL;
extern int const PAINT_SAMPLES_CHANNEL;



static void set_bland_mode(SDL_Texture *texture){
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
}


int init_paint(PermanentVars const *pv, SDL_Renderer *renderer) {
    int isError = 0;

    isError |= load_texture(renderer, PAINT_TEXTURE_FILE, &g_paintTexture, &(g_paint.rect.w), &(g_paint.rect.h), NULL);
    isError |= load_textures(renderer, PAINTED_DISPLAY_FILES_COUNT, PAINTED_DISPLAY_TEXTURE_FILES, g_paintedDisplayTextures, set_bland_mode);
    isError |= load_sample(PAINT_THROW_SAMPLE_FILE, &g_paintThrowSample);


    g_paintedDisplay.rect.w = pv->w;
    g_paintedDisplay.rect.h = pv->h;

    return isError;
}


static void update_paint_rect(SDL_Rect *rect){
    update_rect(rect, MAX_PAINT_REDUCTION, false, 1);
}


bool check_paint_collision(SDL_Rect const *snakeHeadRect) {
    SDL_Rect paintRect = g_paint.rect;
    update_paint_rect(&paintRect);
    return SDL_HasIntersection(&paintRect, snakeHeadRect);
}


static void set_random_paint_position(PermanentVars const *pv, bool excludeFirstLineLine) {
    int paintLine, paintColumn;
    set_random_free_cell(&paintLine, &paintColumn, excludeFirstLineLine);
    get_cell(paintLine, paintColumn)->isFree = false;
    set_position(pv, &(g_paint.rect), (float)paintLine + 0.5f, (float)paintColumn + 0.5f);
}


void throw_paint(PermanentVars const *pv) {
    Mix_PlayChannel(PAINT_SAMPLES_CHANNEL, g_paintThrowSample, 0);
    set_random_paint_position(pv, false);
    g_paintedDisplay.textureNumber = rand() % PAINTED_DISPLAY_FILES_COUNT; // NOLINT(cert-msc30-c, cert-msc50-cpp)
    g_startPaintedDisplay = g_framesCount;
}


void clear_paint(PermanentVars const *pv) {
    g_startPaintedDisplay = -1000;
    set_random_paint_position(pv, true);
}


void render_paint(SDL_Renderer *renderer) {
    SDL_Rect paintRect = g_paint.rect;
    update_paint_rect(&paintRect);
    SDL_RenderCopy(renderer, g_paintTexture, NULL, &paintRect);
    int value = (g_framesCount - g_startPaintedDisplay) / PAINTED_DISPLAY_DELAY;
    if (value <= 255){
        SDL_SetTextureAlphaMod(g_paintedDisplayTextures[g_paintedDisplay.textureNumber], 255 - value);
        SDL_RenderCopy(renderer, g_paintedDisplayTextures[g_paintedDisplay.textureNumber], NULL, &g_paintedDisplay.rect);
    }
}


void close_paint() {
    close_texture(&g_paintTexture);
    close_textures(PAINTED_DISPLAY_FILES_COUNT, g_paintedDisplayTextures);
    close_sample(&g_paintThrowSample);
}
