cmake_minimum_required(VERSION 2.8)
project(scroller)
if(APPLE)
  include_directories(/usr/local/include/)
  link_directories(/usr/local/lib/)
endif(APPLE)
add_executable(scroller main.cpp utf8_decode.c)
if(WIN32)
target_link_libraries(scroller SDL2 SDL2main SDL2_ttf SDL2_image)
else(WIN32)
target_link_libraries(scroller SDL2 SDL2_ttf SDL2_image)
endif(WIN32)
install(TARGETS scroller RUNTIME DESTINATION /usr/bin)
