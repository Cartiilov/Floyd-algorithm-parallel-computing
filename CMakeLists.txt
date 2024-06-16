cmake_minimum_required(VERSION 3.5)
project(UPCFloydProject)

set(PTHREADS_COUNT 4 CACHE STRING "Number of pthreads")

set(SOURCE_FILE ../src/upc_floyd.c)

set(OUTPUT_BINARY floyd)

add_custom_command(
    OUTPUT ${OUTPUT_BINARY}
    COMMAND upcc -bupc -Wc,"-O2" -network=udp -pthreads=${PTHREADS_COUNT} ${SOURCE_FILE} -o ${OUTPUT_BINARY}
    DEPENDS ${SOURCE_FILE}
    COMMENT "Compiling ${SOURCE_FILE} with UPC"
)

add_custom_target(
    compile_floyd ALL
    DEPENDS ${OUTPUT_BINARY}
)

set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES ${OUTPUT_BINARY})