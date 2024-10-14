cmake_minimum_required(VERSION 3.16.0)
set (CMAKE_CXX_STANDARD 20)
project(dyMsg2Repacker LANGUAGES C CXX)

set(EXECUTABLE_NAME ${PROJECT_NAME})

if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
endif()

add_executable(${EXECUTABLE_NAME})

target_sources(${EXECUTABLE_NAME} PRIVATE
    msg2.cpp
)

target_link_libraries(${EXECUTABLE_NAME})
