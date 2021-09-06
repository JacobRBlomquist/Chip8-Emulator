cmake_minimum_required(VERSION 3.7.0)
project(Chip8-Emulator)

set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
add_subdirectory(external/sdl)

add_executable(Chip8
                "src/main.cpp"
                "src/Chip8.cpp"
                )           


target_include_directories(Chip8 PUBLIC "$(CMAKE_SOURCE_DIR)/external/sdl/include")
target_link_libraries(Chip8 SDL2main SDL2-static)