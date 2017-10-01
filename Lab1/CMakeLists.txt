cmake_minimum_required(VERSION 3.8)

project(model)

include_directories(/usr/local/systemc-2.3.1/include/)

add_custom_target(model ALL
        COMMAND make -C ${model_SOURCE_DIR}
        CLION_EXE_DIR=${PROJECT_BINARY_DIR}
        SOURCES src/)

set(SOURCE_FILES src/main.cpp)
add_executable(SystemC ${SOURCE_FILES})
