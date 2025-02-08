cmake_minimum_required(VERSION 3.24)
project(z5 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -lm")

add_executable(z5 src/z5.c src/monopoly.c)
target_include_directories(z5 PRIVATE ${PROJECT_SOURCE_DIR}/include)

