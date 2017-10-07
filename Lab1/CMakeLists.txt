cmake_minimum_required(VERSION 3.8)
project(Lab1)

include_directories(/usr/local/systemc-2.3.1/include)
link_directories(/usr/local/systemc-2.3.1/lib-linux64)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -Wall -pedantic -Wno-long-long -Wno-deprecated-declarations -DSC_INCLUDE_DYNAMIC_PROCESSES -fpermissive ")

set(SOURCE_FILES
        src/main.cpp
        src/cpu.h
        src/bus_matrix.h
        src/mux.h
        src/timer.h
        src/input_capture.h
        src/test_oscillator.h)

add_executable(Lab1 ${SOURCE_FILES})
target_link_libraries(Lab1 systemc m)
