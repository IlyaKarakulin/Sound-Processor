cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(sound_processor_lib)
add_library(sound_processor_lib STATIC sound_pr.cpp sound_pr.hpp reverbConv.cpp)
