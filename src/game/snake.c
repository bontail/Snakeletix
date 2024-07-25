#include <SDL2/SDL.h>
#include <limits.h>
#include "../utils.h"
#include "snake.h"
#include "../game/items/apple.h"
#include "../game/items/portals.h"
#include "../game/items/bombs.h"
#include "../game/items/paint.h"
#include "field.h"
#include "../text.h"



#define IS_VERTICAL(direction) (direction % 2 == 1)
#define IS_HORIZONTAL(direction) (direction % 2 == 0)
#define IS_VERTICAL_PART(index) IS_VERTICAL(g_snake[index].direction)
#define IS_HORIZONTAL_PART(index) IS_HORIZONTAL(g_snake[index].direction)


extern int g_framesCount, g_gameMode;
extern int g_maxScore;
extern int g_bombsCount;


int const GAME_PAUSED_FRAMES_COUNT = 120; // Frames count after dead and before come back to menu
int CAN_QUIET_GAME_FRAMES_COUNT;


enum SnakePartTypes {
    HEAD,
    BODY,
    TAIL,
    SnakePartTypes_COUNT,
};


static char const *const SNAKE_PART_TEXTURE_FILES[SnakePartTypes_COUNT] = {
    "../staticfiles/snake/snakeHead.png", "../staticfiles/snake/snakeBody.png", "../staticfiles/snake/snakeTail.png",
};
static SDL_Texture *g_snakePartTextures[SnakePartTypes_COUNT];


static char const *const SNAKE_PATCH_TEXTURE_FILE = "../staticfiles/snake/patch.png";
static SDL_Texture *g_snakePatchTexture;
static struct SnakePatch{
    SDL_Rect rect;
} *g_snakePatches;
static int g_maxSnakePatchesLength;


static struct SnakePart {
    SDL_Rect rect;
    int countBeforeUsing;
    enum SnakePartTypes type;
    enum Directions direction;
} *g_snake;
int const BODY_PARTS = 2;
int const START_SNAKE_LENGTH = 2;
int g_snakeLength = START_SNAKE_LENGTH * BODY_PARTS;
static int g_maxSnakeLength;


static int const START_SPEED = 10;
static int g_speedValue = START_SPEED;
static int g_countBeforeUsing;


static char const *const GAME_MUSIC_FILE = "../staticfiles/snake/gameMusic.mp3";
static Mix_Music *g_gameMusic = NULL;


static char const *const GAME_WIN_SAMPLE_FILE = "../staticfiles/snake/gameWinSample.wav";
static Mix_Chunk *g_gameWinSample = NULL;
static char const *const GAME_LOSE_SAMPLE_FILE = "../staticfiles/snake/gameLoseSample.wav";
static Mix_Chunk *g_gameLoseSample = NULL;
extern int const GAME_END_SAMPLES_CHANNEL;



static int load_snake_media(SDL_Renderer *renderer) {
    int isError = 0;

    for (int i = 0; i < SnakePartTypes_COUNT; ++i) {
        isError |= load_texture(renderer, SNAKE_PART_TEXTURE_FILES[i], &g_snakePartTextures[i],NULL, NULL,NULL);
    }

    isError |= load_texture(renderer, SNAKE_PATCH_TEXTURE_FILE, &g_snakePatchTexture, &(g_snakePatches[0].rect.w), &(g_snakePatches[0].rect.h), NULL);
    fill_rect_wh(g_snakePatches[0].rect.w, g_snakePatches[0].rect.h, g_snakePatches, sizeof(struct SnakePatch), g_maxSnakePatchesLength);

    isError |= load_music(GAME_MUSIC_FILE, &g_gameMusic);
    isError |= load_sample(GAME_WIN_SAMPLE_FILE, &g_gameWinSample);
    isError |= load_sample(GAME_LOSE_SAMPLE_FILE, &g_gameLoseSample);

    return isError;
}


int init_snake(PermanentVars const *pv, SDL_Renderer *renderer) {
    g_maxSnakeLength = pv->linesCount * pv->columnsCount * BODY_PARTS;
    g_snake = calloc(g_maxSnakeLength, sizeof(struct SnakePart));
    if (g_snake == NULL) {
        printf("Error: Memory limit (g_snake)\n");
        return 1;
    }

    g_maxSnakePatchesLength = pv->linesCount * pv->columnsCount;
    g_snakePatches = calloc(g_maxSnakePatchesLength, sizeof(struct SnakePatch));
    if (g_snakePatches == NULL) {
        printf("Error: Memory limit (g_snakePatches)\n");
        return 1;
    }

    if (load_snake_media(renderer) != 0) {
        return 1;
    }

    return 0;
}


static void create_patch(int const x, int const y) {
    for (int i = 0; i < g_maxSnakePatchesLength; ++i) {
        if (g_snakePatches[i].rect.x == 0 && g_snakePatches[i].rect.y == 0){
            g_snakePatches[i].rect.x = x;
            g_snakePatches[i].rect.y = y;
            break;
        }
    }
}


static void delete_patch(int const x, int const y) {
    for (int i = 0; i < g_maxSnakePatchesLength; ++i) {
        if (g_snakePatches[i].rect.x == x && g_snakePatches[i].rect.y == y) {
            g_snakePatches[i].rect.x = 0;
            g_snakePatches[i].rect.y = 0;
            break;
        }
    }
}


static void stop_snake(bool isWin){
    g_speedValue = 0;
    g_maxScore = MAX(g_maxScore, (g_snakeLength / BODY_PARTS - 2) * (g_gameMode + 1));
    CAN_QUIET_GAME_FRAMES_COUNT = g_framesCount + GAME_PAUSED_FRAMES_COUNT;
    set_render_end_game_text_function(isWin);
    if (isWin) {
        Mix_PlayChannel(GAME_END_SAMPLES_CHANNEL, g_gameWinSample, 0);
    }else {
        Mix_PlayChannel(GAME_END_SAMPLES_CHANNEL, g_gameLoseSample, 0);
    }
}


static void query_part(int index, PermanentVars const *pv, int *relativeX, int *relativeY, int *line, int *column){
    *relativeX = g_snake[index].rect.x - pv->wIndentation;
    *relativeY = g_snake[index].rect.y - pv->hIndentation;
    *line = *relativeY / pv->cellEdge;
    *column = *relativeX / pv->cellEdge;
}


void set_snake_direction(PermanentVars const *pv, enum Directions direction) {
    int relativeX, relativeY, line, column;
    query_part(g_snakeLength - 1, pv, &relativeX, &relativeY, &line, &column);
    if (get_cell(line, column)->direction == NULL_DIRECTION && IS_VERTICAL_PART(g_snakeLength - 1) != IS_VERTICAL(direction)) {
        get_cell(line, column)->direction = direction;
    }
}


static void make_direction(PermanentVars const *pv, int index, int line, int column, enum Directions direction){
    g_snake[index].direction = direction;
    int patchX = (int)((column + 0.5) * pv->cellEdge + pv->wIndentation);
    int patchY = (int)((line + 0.5) * pv->cellEdge + pv->hIndentation);
    if (g_snake[index].type == HEAD){
        create_patch(patchX, patchY);
    }else if(g_snake[index].type == TAIL){
        get_cell(line, column)->direction = NULL_DIRECTION;
        delete_patch(patchX, patchY);
    }
}


static void make_snake_direction(PermanentVars const *pv, int const i) {
    int relativeX, relativeY, line, column;
    query_part(i, pv, &relativeX, &relativeY, &line, &column);
    enum Directions cellDirection = get_cell(line, column)->direction;

    if (cellDirection != NULL_DIRECTION && cellDirection != CLOSE_DIRECTION && (g_snake[i].direction % 2) != (cellDirection % 2)) {
        int delta, toEdgeTexture = g_snake[i].rect.w / 2;
        switch (g_snake[i].direction) {
            case RIGHT:
                if ((column + 0.5) * pv->cellEdge < relativeX + toEdgeTexture) {
                    delta = relativeX - (int) ((column + 0.5) * pv->cellEdge);
                    g_snake[i].rect.x -= delta;

                    if (cellDirection == UP) {
                        g_snake[i].rect.y -= delta;
                    } else if (cellDirection == DOWN) {
                        g_snake[i].rect.y += delta;
                    }
                    make_direction(pv, i, line, column, cellDirection);
                }
                break;
            case DOWN:
                if ((line + 0.5) * pv->cellEdge < relativeY + toEdgeTexture) {
                    delta = relativeY - (int) ((line + 0.5) * pv->cellEdge);
                    g_snake[i].rect.y -= delta;

                    if (cellDirection == RIGHT) {
                        g_snake[i].rect.x += delta;
                    } else if (cellDirection == LEFT) {
                        g_snake[i].rect.x -= delta;
                    }
                    make_direction(pv, i, line, column, cellDirection);
                }
                break;
            case LEFT:
                if ((column + 0.5) * pv->cellEdge > relativeX - toEdgeTexture) {
                    delta = (int) ((column + 0.5) * pv->cellEdge) - relativeX;
                    g_snake[i].rect.x += delta;

                    if (cellDirection == UP) {
                        g_snake[i].rect.y -= delta;
                    } else if (cellDirection == DOWN) {
                        g_snake[i].rect.y += delta;
                    }
                    make_direction(pv, i, line, column, cellDirection);
                }
                break;
            case UP:
                if ((line + 0.5) * pv->cellEdge > relativeY - toEdgeTexture) {
                    delta = (int) ((line + 0.5) * pv->cellEdge) - relativeY;
                    g_snake[i].rect.y += delta;

                    if (cellDirection == RIGHT) {
                        g_snake[i].rect.x += delta;
                    } else if (cellDirection == LEFT) {
                        g_snake[i].rect.x -= delta;
                    }
                    make_direction(pv, i, line, column, cellDirection);
                }
                break;
            default:
                printf("Illegal direction: %d\n", g_snake[i].direction);
                exit(1);
        }
    }
    if (g_snake[i].type == TAIL) {
        get_cell(line, column)->isFree = true;
    }else if(g_snake[i].type == HEAD){
        get_cell(line, column)->isFree = false;
    }
}


void move_snake(PermanentVars const *pv) {
    for (int i = 0; i < g_snakeLength; ++i) {
        if ((g_snake[i].type == TAIL && g_snake[i + 1].countBeforeUsing != 0) || (g_snake[i].countBeforeUsing == g_countBeforeUsing && g_snake[i + 1].countBeforeUsing != 0)) {
            continue;
        }
        g_snake[i].countBeforeUsing = MAX(0, --g_snake[i].countBeforeUsing);
        make_snake_direction(pv, i);
        bool needPortal = false;
        int pos;
        switch (g_snake[i].direction) {
            case RIGHT:
                g_snake[i].rect.x += g_speedValue;
                pos = g_snake[i].rect.x - pv->wIndentation + g_snake[i].rect.w / 2;
                if (pos >= pv->wArea) {
                    needPortal = true;
                    g_snake[i].rect.x = pv->wIndentation + g_snake[i].rect.w / 2 + (pos % pv->wArea);
                }
                break;
            case DOWN:
                g_snake[i].rect.y += g_speedValue;
                pos = g_snake[i].rect.y - pv->hIndentation + g_snake[i].rect.w / 2;
                if (pos >= pv->hArea) {
                    needPortal = true;
                    g_snake[i].rect.y = pv->hIndentation + g_snake[i].rect.w / 2 + (pos % pv->hArea);

                }
                break;
            case LEFT:
                g_snake[i].rect.x -= g_speedValue;
                pos = g_snake[i].rect.x - pv->wIndentation - g_snake[i].rect.w / 2;
                if (pos <= 0) {
                    needPortal = true;
                    g_snake[i].rect.x = pv->wIndentation + pv->wArea - g_snake[i].rect.w / 2 + pos;
                }
                break;
            case UP:
                g_snake[i].rect.y -= g_speedValue;
                pos = g_snake[i].rect.y - pv->hIndentation - g_snake[i].rect.w / 2;
                if (pos <= 0) {
                    needPortal = true;
                    g_snake[i].rect.y = pv->hIndentation + pv->hArea - g_snake[i].rect.w / 2 + pos;
                }
                break;
            default:
                printf("Illegal direction: %d\n", g_snake[i].direction);
                exit(1);
        }
        if (needPortal && can_create_portal()){
            int fistPortalX, firstPortalY, secondPortalX, secondPortalY;
            enum Directions firstPortalDirection, secondPortalDirection;
            if (IS_HORIZONTAL_PART(i)){
                firstPortalDirection = RIGHT;
                secondPortalDirection = LEFT;
                fistPortalX = pv->wArea + pv->wIndentation;
                firstPortalY = g_snake[i].rect.y;
                secondPortalX = fistPortalX - pv->wArea;
                secondPortalY = firstPortalY;
            }else{
                firstPortalDirection = DOWN;
                secondPortalDirection = UP;
                fistPortalX = g_snake[i].rect.x;
                firstPortalY = pv->hArea + pv->hIndentation;
                secondPortalX = fistPortalX;
                secondPortalY = firstPortalY - pv->hArea;

            }
            if (g_snake[i].type == HEAD) {
                create_portal(fistPortalX, firstPortalY, firstPortalDirection);
                create_portal(secondPortalX, secondPortalY, secondPortalDirection);
            }else if (g_snake[i].type == TAIL){
                delete_portal(fistPortalX, firstPortalY);
                delete_portal(secondPortalX, secondPortalY);
            }
        }
    }
}


static void update_snake_part_rect(SDL_Rect *snakePartRect, int const i) {
    if (IS_HORIZONTAL_PART(i)) {
        snakePartRect->x = g_snake[i].rect.x - g_snake[i].rect.w / 2;
        snakePartRect->y = g_snake[i].rect.y - g_snake[i].rect.h / 2;
    } else {
        snakePartRect->x = g_snake[i].rect.x - g_snake[i].rect.h / 2;
        snakePartRect->y = g_snake[i].rect.y - g_snake[i].rect.w / 2;
    }
}


bool check_collision_snake(SDL_Rect *snakeHeadRect) {
    SDL_Rect snakePartRect;
    for (int index = 0; index < g_snakeLength - BODY_PARTS; ++index) {
        snakePartRect = g_snake[index].rect;
        update_snake_part_rect(&snakePartRect, index);
        if (SDL_HasIntersection(snakeHeadRect, &snakePartRect)) {
            return true;
        }
    }
    return false;
}


bool check_end_game_collision(){
    SDL_Rect snakeHead = g_snake[g_snakeLength - 1].rect;
    update_snake_part_rect(&snakeHead, g_snakeLength - 1);
    bool isEndGame = check_collision_snake(&snakeHead);
    if (check_bomb_collision(&snakeHead)){
        detonate_bomb();
        isEndGame |= 1;
    }
    return isEndGame;
}


void update_length() {
    for (int i = g_snakeLength - 1; i > 0; --i) {
        g_snake[i + BODY_PARTS] = g_snake[i];
    }

    for (int i = 1; i < 1 + BODY_PARTS; ++i) {
        g_snake[i].direction = g_snake[0].direction;
        g_snake[i].rect.x = g_snake[0].rect.x;
        g_snake[i].rect.y = g_snake[0].rect.y;
        g_snake[i].rect.w = g_snake[g_snakeLength - 1 + BODY_PARTS].rect.w;
        g_snake[i].rect.h = g_snake[g_snakeLength - 1 + BODY_PARTS].rect.h;
        g_snake[i].type = BODY;
        g_snake[i].countBeforeUsing = g_countBeforeUsing;
    }
    g_snakeLength += BODY_PARTS;
    if (g_snakeLength >= g_maxSnakeLength - g_bombsCount * BODY_PARTS - 1 * BODY_PARTS){
        printf("End");
        stop_snake(true);
    }
}


void make_new_snake_frame(PermanentVars const *pv) {
    if (g_speedValue != 0) {
        move_snake(pv);
        SDL_Rect snakeHeadRect = g_snake[g_snakeLength - 1].rect;
        update_snake_part_rect(&snakeHeadRect, g_snakeLength - 1);
        if (check_apple_collision(&snakeHeadRect)) {
            update_length();
            eat_apple(pv);
        }
        if (check_paint_collision(&snakeHeadRect)) {
            throw_paint(pv);
        }
        if (check_end_game_collision()) {
            stop_snake(false);
        }
    }
}


bool can_quit_game() {
    return CAN_QUIET_GAME_FRAMES_COUNT < g_framesCount;
}


static void build_start_snake(PermanentVars const *pv) {
    g_snake[0].type = TAIL;
    g_snake[0].direction = RIGHT;
    SDL_QueryTexture(g_snakePartTextures[g_snake[0].type], NULL, NULL,  &(g_snake[0].rect.w),  &(g_snake[0].rect.h));
    g_snake[0].rect.x = pv->wIndentation + g_snake[0].rect.w / 2;
    g_snake[0].rect.y = pv->hIndentation + pv->cellEdge / 2;
    g_snake[0].countBeforeUsing = 0;

    for (int i = 1; i < g_snakeLength; ++i) {
        g_snake[i].type = BODY;
        g_snake[i].direction = RIGHT;
        SDL_QueryTexture(g_snakePartTextures[g_snake[i].type], NULL, NULL,  &(g_snake[i].rect.w),  &(g_snake[i].rect.h));
        g_snake[i].rect.x = g_snake[i - 1].rect.x + g_snake[i].rect.w;
        g_snake[i].rect.y = g_snake[0].rect.y;
        g_snake[i].countBeforeUsing = 0;
    }

    g_snake[g_snakeLength - 1].type = HEAD;
    SDL_QueryTexture(g_snakePartTextures[g_snake[g_snakeLength - 1].type], NULL, NULL,  &(g_snake[g_snakeLength - 1].rect.w),  &(g_snake[g_snakeLength - 1].rect.h));
    g_snake[g_snakeLength - 1].rect.x = g_snake[g_snakeLength - 2].rect.x + g_snake[g_snakeLength - 1].rect.w;

    for (int i = 0; i < g_snakeLength / BODY_PARTS; ++i) {
        get_cell(0, i)->isFree = false;
    }

    g_countBeforeUsing = g_snake[1].rect.w / g_speedValue;
}


void clear_snake(PermanentVars const *pv) {
    for (int i = 0; i < g_maxSnakePatchesLength; ++i) {
        g_snakePatches[i].rect.x = 0;
        g_snakePatches[i].rect.y = 0;
    }
    g_snakeLength = START_SNAKE_LENGTH * BODY_PARTS;
    g_speedValue = START_SPEED;
    build_start_snake(pv);
    CAN_QUIET_GAME_FRAMES_COUNT = INT_MAX;
    Mix_HaltMusic();
    Mix_PlayMusic(g_gameMusic, -1);
}



void render_snake(SDL_Renderer *renderer) {
    for (int i = 0; i < g_maxSnakePatchesLength; ++i) {
        if (g_snakePatches[i].rect.x != 0 && g_snakePatches[i].rect.y != 0){
            SDL_Rect patchRect = g_snakePatches[i].rect;
            update_dynamic_rect(&patchRect);
            SDL_RenderCopy(renderer, g_snakePatchTexture, NULL, &patchRect);
        }
    }
    for (int i = 0; i < g_snakeLength; ++i) {
        SDL_Rect snakeRect = g_snake[i].rect;
        update_dynamic_rect(&snakeRect);
        SDL_RenderCopyEx(renderer, g_snakePartTextures[g_snake[i].type], NULL, &snakeRect,
                         90 * g_snake[i].direction, NULL, SDL_FLIP_NONE);
    }
}


void close_snake() {
    for (int i = 0; i < SnakePartTypes_COUNT; ++i) {
        close_texture(&g_snakePartTextures[i]);
    }

    free(g_snake);
    g_snake = NULL;

    free(g_snakePatches);
    g_snakePatches = NULL;

    close_music(&g_gameMusic);
    close_sample(&g_gameWinSample);
    close_sample(&g_gameLoseSample);
}