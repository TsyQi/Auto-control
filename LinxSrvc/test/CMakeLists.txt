cmake_minimum_required(VERSION 2.8...3.16)

set(TARGET test)

project("${TARGET}")
set(CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -ggdb -g -fprofile-arcs -ftest-coverage")

add_subdirectory(googletest)

include_directories(googletest/googletest/include/gtest)
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ../include
    ../webev
)

set(TEST_FILE
    ../webev/Utils.cpp
    test_main.cpp
)

foreach(lib libgmock_main.a libgmock.a libgtest_main.a libgtest.a)
    if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/build/lib/${lib})
        set(LIBS_LACK TRUE)
    endif()
endforeach(lib libgmock_main.a libgmock.a libgtest_main.a libgtest.a)

if(${LIBS_LACK})
    include_directories(googletest/googletest/include/gtest)
endif()

file(GLOB SRC_FILE_LIST "tests/test*_*.c*")
add_executable(${TARGET}
    ${SRC_FILE_LIST}
    ${TEST_FILE}
)

link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
target_link_libraries(${TARGET}
    gmock_main
    gmock
    gtest_main
    gtest
)
