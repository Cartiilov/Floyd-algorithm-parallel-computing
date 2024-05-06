cmake_minimum_required(VERSION 3.5)
project(FloydProject)

execute_process(COMMAND bash ${CMAKE_SOURCE_DIR}/load_settings.sh)

include_directories(include)

add_executable(floyd src/main.c src/floyd.c)

add_executable(graph_viz src/graph_viz.c)


find_package(MPI REQUIRED)
target_link_libraries(floyd MPI::MPI_C)
target_link_libraries(graph_viz MPI::MPI_C)


target_link_libraries(floyd m)
target_link_libraries(graph_viz m)

#find_package(MPE REQUIRED)
target_compile_options(graph_viz PUBLIC "-I/opt/nfs/mpe2-2.4.9b/include")
target_link_options(graph_viz PUBLIC "-L/opt/nfs/mpe2-2.4.9b/lib")
target_link_libraries(graph_viz mpe)
target_link_libraries(graph_viz X11)

set(CMAKE_C_COMPILER mpicc)

option(test "test the corectness of Floyd's algorithm" OFF)

if(test)
    add_custom_target(RunPythonScript ALL
        COMMAND python3 ${CMAKE_SOURCE_DIR}/compare.py
        COMMENT "Running the testing"
    )
endif()