#include <SDL2/SDL.h>
#include <SDL_mixer.h>
#include "../../utils.h"



static char const *const PORTAL_TEXTURE_FILES[] = {
        "../staticfiles/items/portals/portal1.png", "../staticfiles/items/portals/portal2.png",
        "../staticfiles/items/portals/portal3.png",
        "../staticfiles/items/portals/portal4.png", "../staticfiles/items/portals/portal5.png",
        "../staticfiles/items/portals/portal6.png"
};
#define PORTAL_TEXTURE_FILES_COUNT (int)(sizeof(PORTAL_TEXTURE_FILES) / sizeof(char const *))
static SDL_Texture *g_portalTextures[PORTAL_TEXTURE_FILES_COUNT];
#define MAX_PORTALS_COUNT 8
static struct Portal{
    SDL_Rect rect;
    enum Directions direction;
    int number;
} g_portals[MAX_PORTALS_COUNT];
int g_maxSessionPortalsCount;
int g_currentPortalsCount;


static char const *const CREATE_PORTAL_SAMPLE_FILE = "../staticfiles/items/portals/portalSample.wav";
static Mix_Chunk *g_createPortalSample = NULL;
extern int const PORTAL_SAMPLES_CHANNEL;



int init_portals(SDL_Renderer *renderer) {
    int isError = 0;

    isError |= load_textures(renderer, PORTAL_TEXTURE_FILES_COUNT, PORTAL_TEXTURE_FILES, g_portalTextures, NULL);
    SDL_QueryTexture(g_portalTextures[0], NULL, NULL, &(g_portals[0].rect.w), &(g_portals[0].rect.h));
    fill_rect_wh(g_portals[0].rect.w, g_portals[0].rect.h, g_portals, sizeof(struct Portal), MAX_PORTALS_COUNT);
    isError |= load_sample(CREATE_PORTAL_SAMPLE_FILE, &g_createPortalSample);

    return isError;
}


void clear_portals(enum GameModes gameMode) {
    g_maxSessionPortalsCount = MAX_PORTALS_COUNT / (gameMode + 1);
    g_currentPortalsCount = 0;
    for (int i = 0; i < g_maxSessionPortalsCount; ++i) {
        g_portals[i].rect.x = 0;
        g_portals[i].rect.y = 0;
    }
}


bool can_create_portal() {
    for (int i = 0; i < g_maxSessionPortalsCount; ++i) {
        if (g_portals[i].rect.x == 0 && g_portals[i].rect.y == 0) {
            return true;
        }
    }
    return false;
}


void create_portal(int const x, int const y, enum Directions direction) {
    for (int i = 0; i < g_maxSessionPortalsCount; ++i) {
        if (g_portals[i].rect.x == 0 && g_portals[i].rect.y == 0) {
            g_portals[i].rect.x = x;
            g_portals[i].rect.y = y;
            g_portals[i].direction = direction;
            g_portals[i].number = 0;
            Mix_PlayChannel(PORTAL_SAMPLES_CHANNEL, g_createPortalSample, 0);
            g_currentPortalsCount++;
            break;
        }
    }
}


void delete_portal(int const x, int const y) {
    for (int i = 0; i < g_maxSessionPortalsCount; ++i) {
        if (g_portals[i].rect.x == x && g_portals[i].rect.y == y) {
            g_portals[i].rect.x = 0;
            g_portals[i].rect.y = 0;
            g_currentPortalsCount--;
            break;
        }
    }
}


void render_portals(SDL_Renderer *renderer) {
    for (int i = 0; i < g_maxSessionPortalsCount; ++i) {
        if (g_portals[i].rect.x != 0 && g_portals[i].rect.y != 0) {
            SDL_Rect portalRect = g_portals[i].rect;
            update_dynamic_rect(&portalRect);

            SDL_RendererFlip flip = SDL_FLIP_NONE;
            if (g_portals[i].direction == LEFT || g_portals[i].direction == DOWN) {
                flip = SDL_FLIP_VERTICAL;
            }
            SDL_RenderCopyEx(renderer, g_portalTextures[g_portals[i].number], NULL, &portalRect,
                             90 * (g_portals[i].direction % 2), NULL, flip);
            g_portals[i].number = (g_portals[i].number + 1) % PORTAL_TEXTURE_FILES_COUNT;
        }
    }
}


void close_portals() {
    close_textures(PORTAL_TEXTURE_FILES_COUNT, g_portalTextures);
    close_sample(&g_createPortalSample);
}