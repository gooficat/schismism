cmake_minimum_required(VERSION 3.10.0)
project(Schism VERSION 0.1.0 LANGUAGES C)


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

find_package(SDL2 REQUIRED)
find_package(SDL2_Image REQUIRED)

file(GLOB SOURCES src/*.c)

add_executable(Schism ${SOURCES})

target_compile_options(Schism PRIVATE -Wall -Werror)

target_link_libraries(Schism
    SDL2
    SDL2_Image
    enet
)
