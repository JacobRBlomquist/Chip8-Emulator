cmake_minimum_required(VERSION 3.7.0)
project(sdl-demo)

set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
add_subdirectory(external/sdl)

add_executable(sdl2-demo
                "src/main.cpp"
                "src/Chip8.cpp"
                )           


target_include_directories(sdl2-demo PUBLIC "$(CMAKE_SOURCE_DIR)/external/sdl/include")
target_link_libraries(sdl2-demo SDL2main SDL2-static)