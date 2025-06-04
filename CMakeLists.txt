cmake_minimum_required(VERSION 3.28)
project(GameOfLife LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable(gol 
    src/main.cpp
    src/core/GameEngine.cpp
    src/core/Grid.cpp
    src/graphics/Renderer.cpp
    src/input/InputHandler.cpp
    src/ui/UIManager.cpp
    src/ui/Button.cpp
    src/patterns/PatternManager.cpp
)
target_compile_features(gol PRIVATE cxx_std_17)
target_link_libraries(gol PRIVATE SFML::Graphics)
