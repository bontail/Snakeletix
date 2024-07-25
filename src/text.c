#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include "main.h"



extern int g_framesCount, g_gameMode;
extern int g_snakeLength, BODY_PARTS, START_SNAKE_LENGTH;
extern int g_currentPortalsCount, g_maxSessionPortalsCount;


static char const *const USERDATA_FILE = "../userdata/score.txt";
int g_maxScore = 0;


static char const *const FONT_FILE = "../staticfiles/text/font.ttf";
static TTF_Font *g_font = NULL;


static SDL_Color const WHITE = {255, 255, 255};
static SDL_Color const RED = {255, 0, 0};
static SDL_Color const YELLOW = {255, 255, 0};


static SDL_Rect FPS_MESSAGE_RECT = {0, 0, 0, 0};
static SDL_Rect SCORE_MESSAGE_RECT = {0, 0, 0, 0};
static SDL_Rect PORTALS_COUNT_MESSAGE_RECT = {0, 0, 0, 0};
static SDL_Rect MAX_SCORE_MESSAGE_RECT = {0, 0, 0, 0};
static SDL_Rect END_GAME_MESSAGE_RECT = {0, 0, 0, 0};


static void (*g_renderEndGameTextFunction)(SDL_Renderer *);



static void read_userdata_file() {
    FILE *file = fopen(USERDATA_FILE, "a+");
    rewind(file); // set cursor to start
    char buffer[4]; // 4 because: 3 chars - max score, 1 - '\n'
    int i = 0;
    while((buffer[i++] = (char)getc(file)) != EOF) {}
    fclose(file);
    int fileValue;
    sscanf(buffer, "%d", &fileValue);
    g_maxScore += fileValue;
}


static void write_userdata_file() {
    FILE *file = fopen(USERDATA_FILE, "w");
    int size = snprintf(NULL, 0, "%d", g_maxScore);
    char *buffer = malloc(size);
    sprintf(buffer, "%d", g_maxScore);
    for (int i = 0; i < size; ++i) {
        putc(buffer[i], file);
    }
    putc('\n', file);
    fclose(file);
    free(buffer);
}


int init_text(PermanentVars const *pv) {
    g_font = TTF_OpenFont(FONT_FILE, 24);
    if (g_font == NULL) {
        printf("TTF_OpenFont Error: %s\n", SDL_GetError());
        return 1;
    }
    read_userdata_file();

    FPS_MESSAGE_RECT.w = pv->w / 19;
    FPS_MESSAGE_RECT.h = FPS_MESSAGE_RECT.w / 2;

    SCORE_MESSAGE_RECT.w = pv->w / 9;
    SCORE_MESSAGE_RECT.h = SCORE_MESSAGE_RECT.w / 3;
    SCORE_MESSAGE_RECT.x = pv->w / 2 - SCORE_MESSAGE_RECT.w / 2;
    SCORE_MESSAGE_RECT.y = pv->hIndentation / 2 - SCORE_MESSAGE_RECT.h / 2;

    PORTALS_COUNT_MESSAGE_RECT.w = pv->w / 8;
    PORTALS_COUNT_MESSAGE_RECT.h = PORTALS_COUNT_MESSAGE_RECT.w / 4;
    PORTALS_COUNT_MESSAGE_RECT.x = pv->wIndentation;
    PORTALS_COUNT_MESSAGE_RECT.y = SCORE_MESSAGE_RECT.y;

    MAX_SCORE_MESSAGE_RECT.w = pv->w / 8;
    MAX_SCORE_MESSAGE_RECT.h = MAX_SCORE_MESSAGE_RECT.w / 4;
    MAX_SCORE_MESSAGE_RECT.x = pv->w / 2 - MAX_SCORE_MESSAGE_RECT.w / 2;
    MAX_SCORE_MESSAGE_RECT.y = pv->hIndentation / 2 - MAX_SCORE_MESSAGE_RECT.h / 2;


    END_GAME_MESSAGE_RECT.w = pv->w / 3;
    END_GAME_MESSAGE_RECT.h = pv->h / 3;
    END_GAME_MESSAGE_RECT.x = pv->w / 2 - END_GAME_MESSAGE_RECT.w / 2;
    END_GAME_MESSAGE_RECT.y = pv->h / 2 - END_GAME_MESSAGE_RECT.h / 2;

    return 0;
}


void render_text(SDL_Renderer *renderer, char *firstPart, char *secondPart, int value, SDL_Rect messageRect, SDL_Color color) {
    if (secondPart != NULL) {
        sprintf(secondPart, "%d", value);
        strcat(firstPart, secondPart);
    }
    SDL_Surface *surfaceMessage = TTF_RenderText_Solid(g_font, firstPart, color);
    SDL_Texture *message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    SDL_RenderCopy(renderer, message, NULL, &messageRect);
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(message);
}


void render_fps_text(SDL_Renderer *renderer, int const startTicks) {
    char textMessageFPS[8] = "FPS ";
    char valueFPS[4];
    int const fps = g_framesCount / (int)((SDL_GetTicks() - startTicks) / 1000.0);
    render_text(renderer, textMessageFPS, valueFPS, fps, FPS_MESSAGE_RECT, WHITE);
}


void render_score_text(SDL_Renderer *renderer) {
    char textMessageScore[10] = "Score: ";
    char valueScore[3];
    int const score = (g_snakeLength / BODY_PARTS - START_SNAKE_LENGTH) * (g_gameMode + 1);
    render_text(renderer, textMessageScore, valueScore, score, SCORE_MESSAGE_RECT, WHITE);
}


void render_portals_count_text(SDL_Renderer *renderer) {
    char textMessageScore[16] = "Portals count:";
    char valueScore[1];
    int const portalsCount = g_maxSessionPortalsCount - g_currentPortalsCount;
    render_text(renderer, textMessageScore, valueScore, portalsCount, PORTALS_COUNT_MESSAGE_RECT, WHITE);
}


void render_max_score_text(SDL_Renderer *renderer) {
    char textMessageScore[14] = "Max score: ";
    char valueScore[3];
    render_text(renderer, textMessageScore, valueScore, g_maxScore, MAX_SCORE_MESSAGE_RECT, WHITE);
}


void render_win_game_text(SDL_Renderer *renderer) {
    char textMessageEndGame[] = "You win!";
    render_text(renderer, textMessageEndGame, NULL, 0, END_GAME_MESSAGE_RECT, YELLOW);
}


void render_lose_game_text(SDL_Renderer *renderer) {
    char textMessageEndGame[] = "You lose";
    render_text(renderer, textMessageEndGame, NULL, 0, END_GAME_MESSAGE_RECT, RED);
}


void set_render_end_game_text_function(bool isWin){
    if (isWin){
        g_renderEndGameTextFunction = render_win_game_text;
    }else{
        g_renderEndGameTextFunction = render_lose_game_text;
    }
}


void render_end_game_text(SDL_Renderer *renderer){
    if (g_renderEndGameTextFunction != NULL){
        g_renderEndGameTextFunction(renderer);
    }
}


void clear_text(){
    g_renderEndGameTextFunction = NULL;
}


void close_text() {
    write_userdata_file();
    TTF_CloseFont(g_font);
    g_font = NULL;
}