cmake_minimum_required(VERSION 3.10)
set(This cfTests)
project(${This} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_executable(${This}
    cftests.cpp
)

target_link_libraries(${This} PUBLIC gtest_main LIBcommfinder easyloggingpp)
