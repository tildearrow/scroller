project(scroller)
add_executable(scroller main.cpp)
target_link_libraries(scroller SDL2 SDL2_ttf)
install(TARGETS scroller RUNTIME DESTINATION bin)