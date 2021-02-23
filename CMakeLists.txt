cmake_minimum_required(VERSION 3.10)
set(CMAKE_PROJECT_NAME commfinder)
project(${CMAKE_PROJECT_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(build_static_lib true) # for building and linking easyloggingpp

include(FetchContent)
enable_testing()

FetchContent_Declare(easyloggingpp
    GIT_REPOSITORY https://github.com/amrayn/easyloggingpp.git
    GIT_TAG        v9.96.7
)
FetchContent_MakeAvailable(easyloggingpp)
include_directories(${easyloggingpp_SOURCE_DIR}/src)

FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        release-1.10.0
)
FetchContent_MakeAvailable(googletest)

set(Sources
    src/helpers.cpp
    src/bruteforcesolver.cpp
    src/commutatorfinder.cpp
    src/cubestate.cpp
    src/incrementalscramble.cpp
    src/cube_moves.cpp
    src/searchcriteria.cpp
)

set(Headers
    src/helpers.h
    src/bruteforcesolver.h
    src/commutatorfinder.h
    src/cubestate.h
    src/incrementalscramble.h
    src/cube_moves.h
    src/searchcriteria.h
)

add_library("LIB${CMAKE_PROJECT_NAME}" STATIC ${Sources})
add_subdirectory(test)
target_include_directories("LIB${CMAKE_PROJECT_NAME}" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src" ${easyloggingpp_SOURCE_DIR}/src)
add_executable(${CMAKE_PROJECT_NAME} src/main.cpp)

target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC "LIB${CMAKE_PROJECT_NAME}" easyloggingpp -lpthread)
