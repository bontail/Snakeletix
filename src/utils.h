#ifndef UTILS_H
#define UTILS_H
#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include "main.h"



typedef void (*FunctionAfterLoadType)(SDL_Texture *);


int load_texture(SDL_Renderer *renderer, char const *file, SDL_Texture **texture, int *w, int *h, FunctionAfterLoadType function);
int load_textures(SDL_Renderer *renderer, int count, char const *const filesArray[count], SDL_Texture **texturesArray, FunctionAfterLoadType function);
int load_music(char const *file, Mix_Music **music);
int load_sample(char const *file, Mix_Chunk **sample);

void close_texture(SDL_Texture **texture);
void close_textures(int count, SDL_Texture *textures[count]);
void close_music(Mix_Music **music);
void close_sample(Mix_Chunk **sample);

void fill_rect_wh(int w, int h, void *startPointer, size_t structSize, size_t arrayLength);

void update_dynamic_rect(SDL_Rect *rect);
void update_rect(SDL_Rect *rect, int maxPercent, bool isPlus, int speed);
void set_position(PermanentVars const *pv, SDL_Rect *rect, float line, float column);

#endif //UTILS_H
