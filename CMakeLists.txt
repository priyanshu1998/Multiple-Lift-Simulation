cmake_minimum_required(VERSION 3.20)
project(assignment6 C)

set(CMAKE_C_STANDARD 99)

add_executable(main main.c)
add_executable(Lift Lift.c structdefs.c)
add_executable(Person Person.c structdefs.c)
