cmake_minimum_required(VERSION 3.13)

set(CMAKE_BUILD_TYPE RelWithDebInfo)

project(find_name_game)

add_subdirectory(src/utils)
add_subdirectory(src/window)

set(APP src/app.c)

add_executable(app ${APP})

target_link_libraries(app UTILS WINDOW)