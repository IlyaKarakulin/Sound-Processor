cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(sound_processor VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(lib)

target_include_directories(sound_processor_lib PUBLIC .) # PRIVATE


add_executable(sound_pr main.cpp)
target_link_libraries(sound_pr PRIVATE sound_processor_lib)

