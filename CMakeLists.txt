cmake_minimum_required(VERSION 3.10.0)
project(SoftwareRenderingStarter VERSION 0.1.0 LANGUAGES C)


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

find_package(SDL2 REQUIRED)

file(GLOB SOURCES src/*.c)

add_executable(SoftwareRenderingStarter ${SOURCES})

target_link_libraries(SoftwareRenderingStarter
    SDL2
)
