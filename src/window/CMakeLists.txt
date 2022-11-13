INCLUDE(FindPkgConfig)                                                                                          
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)                                                                               
PKG_SEARCH_MODULE(SDL2TTF REQUIRED SDL2_ttf >= 2.0.0)                                          
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image >= 2.0.0)                                          
set(CMAKE_CXX_STANDARD 17)        
set(CMAKE_CXX_STANDARD_REQUIRED ON)                                     

add_library(WINDOW window.c project.c)

target_link_libraries(WINDOW ${SDL2_INCLUDE_DIRS} ${SDL2TTF_INCLUDE_DIRS})
target_link_libraries(WINDOW ${SDL2_LIBRARIES} ${SDL2TTF_LIBRARIES})
target_link_libraries(WINDOW ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES})