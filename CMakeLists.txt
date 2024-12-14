cmake_minimum_required(VERSION 3.21)
project(RISCV_Simulator)

set(CMAKE_CXX_STANDARD 23)

add_executable(${PROJECT_NAME}
    main.cpp
    src/Machine/machine.cpp
    src/Hart/hart.cpp
    src/Decoder/Decoder.cpp
    src/instruction.cpp
    src/Decoder/Test.cpp
)

set(HEADER_LIST
    "src/Hart"
    "src/Machine"
    "src/Decoder"
    "src"
)

foreach(header ${HEADER_LIST})
    list(REMOVE_ITEM HEADER_LIST ${header})
    list(APPEND HEADER_LIST "${CMAKE_SOURCE_DIR}/${header}")
endforeach()

target_include_directories(${PROJECT_NAME} PUBLIC ${HEADER_LIST})
