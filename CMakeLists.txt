cmake_minimum_required(VERSION 3.1)

set(project_name hw-2)
project(${project_name})

set(CMAKE_C_FLAGS "-g -Wall -Wextra -Werror -Wpedantic -pthread")
set(CMAKE_C_STANDARD 11)


if(ENABLE_PARALLEL)
    add_library(${project_name}parallel_lib SHARED ${PROJECT_SOURCE_DIR}/parallel_lib/src/utils.c)
    set(includes ${PROJECT_SOURCE_DIR}/parallel_lib/include)
    set(library_name ${project_name}parallel_lib)
    set(libsources ${PROJECT_SOURCE_DIR}/parallel_lib/src)
else()
    add_library(${project_name}consistent_lib STATIC ${PROJECT_SOURCE_DIR}/consistent_lib/src/utils.c)
    set(includes ${PROJECT_SOURCE_DIR}/consistent_lib/include)
    set(library_name ${project_name}consistent_lib)
    set(libsources ${PROJECT_SOURCE_DIR}/consistent_lib/src)
endif()

if(ENABLE_COVERAGE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage -lgcov")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lgcov --coverage")
endif()


configure_file(CMakeLists.txt.in
        googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
execute_process(COMMAND ${CMAKE_COMMAND} --build .
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )

add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
        ${CMAKE_BINARY_DIR}/googletest-build)


include_directories(${includes})

enable_testing()
add_subdirectory(test)


add_executable(${project_name} main.c)
target_link_libraries(${project_name} ${library_name})

set(test_write_sources ${PROJECT_SOURCE_DIR}/read_write_test/write_test.c)
set(test_read_sources ${PROJECT_SOURCE_DIR}/read_write_test/read_test.c)

add_executable(write_test ${test_write_sources})
target_link_libraries(write_test ${library_name})


add_executable(read_test ${test_read_sources})
target_link_libraries(read_test ${library_name})