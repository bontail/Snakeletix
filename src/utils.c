#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include "main.h"
#include "utils.h"


extern int g_framesCount;


// loads the texture and makes the usual Query for width and height
int load_texture(SDL_Renderer *renderer, char const *const file, SDL_Texture **texture, int *w, int *h,
                 FunctionAfterLoadType function) {
    *texture = IMG_LoadTexture(renderer, file);
    if (*texture == NULL) {
        printf("IMG_LoadTexture Error: %s (File: %s)\n", SDL_GetError(), file);
        return 1;
    }
    if (w != NULL || h != NULL) {
        SDL_QueryTexture(*texture, NULL, NULL, w, h);
    }
    if (function != NULL) {
        function(*texture);
    }
    return 0;
}


int load_textures(SDL_Renderer *renderer, int const count, char const *const filesArray[count], SDL_Texture **texturesArray,
              FunctionAfterLoadType function) {
    int isError = 0;
    for (int i = 0; i < count; ++i) {
        isError |= load_texture(renderer, filesArray[i], &texturesArray[i], NULL, NULL, function);
    }
    return isError;
}


int load_music(char const *file, Mix_Music **music) {
    *music = Mix_LoadMUS(file);
    if (*music == NULL) {
        printf("SDL_mixer Error: %s (File: %s)\n", Mix_GetError(), file);
        return 1;
    }
    return 0;
}


int load_sample(char const *file, Mix_Chunk **sample) {
    *sample = Mix_LoadWAV(file);
    if (*sample == NULL) {
        printf("SDL_mixer Error: %s (File: %s)\n", Mix_GetError(), file);
        return 1;
    }
    return 0;
}


void close_texture(SDL_Texture **texture) {
    SDL_DestroyTexture(*texture);
    texture = NULL;
}


void close_textures(int const count, SDL_Texture *textures[count]) {
    for (int i = 0; i < count; ++i) {
        close_texture(&textures[i]);
    }
}


void close_music(Mix_Music **music) {
    Mix_FreeMusic(*music);
    music = NULL;
}


void close_sample(Mix_Chunk **sample) {
    Mix_FreeChunk(*sample);
    sample = NULL;
}


// fill w, h in custom struct arrays
// !!! SDL_Rect must be in start of struct !!!
void fill_rect_wh(int w, int h, void *startPointer, size_t const structSize, size_t const arrayLength) {
    for (int i = 0; i < arrayLength; ++i) {
        SDL_Rect *rect = (SDL_Rect *) ((char *) startPointer + (structSize * i));
        rect->w = w;
        rect->h = h;
    }
}


void update_dynamic_rect(SDL_Rect *rect){
    rect->x -= rect->w / 2;
    rect->y -= rect->h / 2;
}


void update_rect(SDL_Rect *rect, int const maxPercent, bool isPlus, int speed) {
    int percent = (g_framesCount * speed) % (maxPercent * 2);
    if (percent >= maxPercent) {
        percent = maxPercent - (percent % maxPercent);
    }
    rect->w = (int)(rect->w / 100.0 * (100 + (percent * (isPlus ? 1 : -1))));
    rect->h = (int)(rect->h / 100.0 * (100 + (percent * (isPlus ? 1 : -1))));
    update_dynamic_rect(rect);
}


void set_position(PermanentVars const *pv, SDL_Rect *rect, float const line, float const column){
    rect->y = pv->hIndentation + (int)(line * (float)pv->cellEdge);
    rect->x = pv->wIndentation + (int)(column * (float)pv->cellEdge);
}