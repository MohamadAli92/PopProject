cmake_minimum_required(VERSION 3.21)
project(PopProject C)

set(CMAKE_C_STANDARD 99)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

add_executable(PopProject main.c modules/welcome.c modules/create.c modules/manage.c modules/utilities.c)
target_link_libraries(PopProject ${CURSES_LIBRARIES})
