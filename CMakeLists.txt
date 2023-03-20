cmake_minimum_required(VERSION 3.4...3.18)
project(my_pybind11 LANGUAGES CXX)

add_subdirectory(extern/pybind11)

pybind11_add_module(example1 example1.cpp)
