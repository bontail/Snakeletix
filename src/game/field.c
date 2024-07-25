#include <SDL2/SDL.h>
#include "../utils.h"
#include "field.h"



static char const *const CELL_TEXTURE_FILES[] = {"../staticfiles/field/cell1.png", "../staticfiles/field/cell2.png"};
#define CELL_TEXTURE_FILES_COUNT (int)(sizeof(CELL_TEXTURE_FILES) / sizeof(char const *))
static SDL_Texture *g_fieldTexture;
static struct Field{
    SDL_Rect rect;
} field;


static Cell *g_cellField;
static int g_linesCount, g_columnsCount;



static int load_field_texture(SDL_Renderer *renderer, SDL_Texture **cellTextures) {
    int isError = 0;
    isError |= load_textures(renderer, CELL_TEXTURE_FILES_COUNT, CELL_TEXTURE_FILES, cellTextures, NULL);

    return isError;
}


static void set_pv(PermanentVars *pv) {
    // any cell texture must have the same height and length
    pv->wIndentation = (pv->w - pv->cellEdge * g_columnsCount) / 2;
    pv->hIndentation = (pv->h - pv->cellEdge * g_linesCount) / 2;
    pv->wArea = pv->w - pv->wIndentation * 2;
    pv->hArea = pv->h - pv->hIndentation * 2;
}


static void create_field_texture(PermanentVars *pv, SDL_Renderer *renderer, SDL_Texture **cellTextures) {
    g_fieldTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
                                       pv->wArea, pv->hArea);
    SDL_SetRenderTarget(renderer, g_fieldTexture); // Now SDL_RenderCopy add textures to g_backgroundTexture
    SDL_Rect cellRect;
    for (int i = 0; i < g_linesCount; ++i) {
        for (int j = 0; j < g_columnsCount; ++j) {
            cellRect.x = pv->cellEdge * j;
            cellRect.y = pv->cellEdge * i;
            cellRect.h = pv->cellEdge;
            cellRect.w = pv->cellEdge;
            SDL_Texture *cellTexture = cellTextures[(i + j) % CELL_TEXTURE_FILES_COUNT];
            SDL_RenderCopy(renderer, cellTexture, NULL, &cellRect);
        }
    }

    SDL_SetRenderTarget(renderer, NULL); // Now SDL_RenderCopy add textures to Display

    field.rect.x = pv->wIndentation;
    field.rect.y = pv->hIndentation;
    field.rect.w = pv->wArea;
    field.rect.h = pv->hArea;
}


int init_field(PermanentVars *pv, SDL_Renderer *renderer) {
    g_linesCount = pv->linesCount;
    g_columnsCount = pv->columnsCount;
    g_cellField = calloc(g_linesCount * g_columnsCount, sizeof(Cell));
    if (g_cellField == NULL) {
        printf("Error: Memory limit (g_cellField)\n");
        return 1;
    }


    SDL_Texture **cellTextures = calloc(CELL_TEXTURE_FILES_COUNT, sizeof(SDL_Texture *));
    if (cellTextures == NULL) {
        printf("Error: Memory limit (g_cellField)\n");
        return 1;
    }

    if (load_field_texture(renderer, cellTextures) != 0) {
        return 1;
    }

    SDL_QueryTexture(cellTextures[0], NULL, NULL, &(pv->cellEdge), NULL); // Set value
    set_pv(pv);

    create_field_texture(pv, renderer, cellTextures);

    close_textures(CELL_TEXTURE_FILES_COUNT, cellTextures);
    free(cellTextures);

    return 0;
}


Cell *get_cell(int line, int column){
    return &g_cellField[line * g_columnsCount + column];
}


void set_random_free_cell(int *line, int *column, bool excludeFirstLine) {
    int suitableLines[g_linesCount], suitableLinesCount = 0;
    for (int i = (int)excludeFirstLine; i < g_linesCount; ++i) {
        for (int j = 0; j < g_columnsCount; ++j) {
            if (get_cell(i, j)->isFree) {
                suitableLines[suitableLinesCount++] = i;
                break;
            }
        }
    }
    *line = suitableLines[rand() % suitableLinesCount]; // NOLINT(cert-msc30-c, cert-msc50-cpp)
    int suitableColumns[g_columnsCount], suitableColumnsCount = 0;
    for (int i = 0; i < g_columnsCount; ++i) {
        if (get_cell(*line,i)->isFree) {
            suitableColumns[suitableColumnsCount++] = i;
        }
    }
    *column = suitableColumns[rand() % suitableColumnsCount]; // NOLINT(cert-msc30-c, cert-msc50-cpp)
}


void clear_field() {
    for (int i = 0; i < g_linesCount; ++i) {
        for (int j = 0; j < g_columnsCount; ++j) {
            g_cellField[i * g_columnsCount + j].direction = NULL_DIRECTION;
            g_cellField[i * g_columnsCount + j].isFree = true;
        }
    }
}


void render_field(SDL_Renderer *renderer) {
    SDL_RenderCopy(renderer, g_fieldTexture, NULL, &field.rect);
}


void close_field() {
    free(g_cellField);
    g_cellField = NULL;

    close_texture(&g_fieldTexture);
}
