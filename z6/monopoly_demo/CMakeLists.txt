cmake_minimum_required(VERSION 3.26)
project(monopoly_ncurses C)

set(CMAKE_C_STANDARD 11)

add_executable(monopoly_ncurses main.c)
target_link_libraries(monopoly_ncurses -lncurses)
