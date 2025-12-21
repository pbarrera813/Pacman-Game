cmake_minimum_required(VERSION 3.11)

project(PacmanGame)

add_executable(pacman Main.cpp Game.cpp Pacman.cpp Ghost.cpp GhostAI.cpp Map.cpp Renderer.cpp TextureManager.cpp AudioManager.cpp)
target_include_directories(pacman PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# sdl2
find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(pacman
        PRIVATE
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)

# sdl2-image
find_package(SDL2_image CONFIG REQUIRED)
target_link_libraries(pacman PRIVATE $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>)

# sdl2-mixer
find_package(SDL2_mixer CONFIG REQUIRED)
target_link_libraries(pacman PRIVATE $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>)

# sdl2-ttf
find_package(SDL2_ttf CONFIG REQUIRED)
target_link_libraries(pacman PRIVATE $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>)