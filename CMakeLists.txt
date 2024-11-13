cmake_minimum_required(VERSION 3.21)
project(RISCV_Simulator)

set(CMAKE_CXX_STANDARD 23)

option(INV_MEMORY_ORDER "Reverse RAM memory order" OFF)

if(INV_MEMORY_ORDER)
    message(NOTICE "Inverse Memory Order is Enabled")
    add_compile_definitions(-DINV_MEMORY_ORDER)
endif()

add_executable(${PROJECT_NAME}
    main.cpp
    src/Machine/machine.cpp
    src/Hart/hart.cpp
    src/Decoder/Decoder.cpp
)

set(HEADER_LIST
    "src/Hart"
    "src/Machine"
    "src/Decoder"
)

foreach(header ${HEADER_LIST})
    list(REMOVE_ITEM HEADER_LIST ${header})
    list(APPEND HEADER_LIST "${CMAKE_SOURCE_DIR}/${header}")
endforeach()

target_include_directories(${PROJECT_NAME} PUBLIC ${HEADER_LIST})
