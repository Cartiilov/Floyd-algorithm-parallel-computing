cmake_minimum_required(VERSION 3.0)
project(MyProject)

execute_process(COMMAND bash ${CMAKE_SOURCE_DIR}/load_settings.sh)

include_directories(include)

add_executable(myprogram src/main.c src/floyd.c)

add_executable(graph_viz graph_viz.c vis.c)


find_package(MPI REQUIRED)
target_link_libraries(myprogram MPI::MPI_C)
target_link_libraries(graph_viz MPI::MPI_C)


target_link_libraries(myprogram m)
target_link_libraries(graph_viz m)

set(CMAKE_C_COMPILER mpicc)