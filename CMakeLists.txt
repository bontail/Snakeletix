cmake_minimum_required(VERSION 3.28)
project(Snakeletix C)

set(CMAKE_C_STANDARD 23)

add_executable(Snakeletix
        src/main.c
        src/text.c
        src/utils.c
        src/menu/menu.c
        src/game/field.c
        src/game/snake.c
        src/game/items/apple.c
        src/game/items/portals.c
        src/game/items/bombs.c
        src/game/items/quicksands.c
        src/game/items/paint.c
)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)
include_directories(Snakeletix ${SDL2_INCLUDE_DIRS} ${SDL2_image_INCLUDE_DIRS} ${SDL2_ttf_INCLUDE_DIRS} ${SDL2_mixer_INCLUDE_DIRS})
target_link_libraries(Snakeletix PRIVATE SDL2::SDL2 SDL2_image::SDL2_image SDL2_ttf::SDL2_ttf SDL2_mixer::SDL2_mixer)