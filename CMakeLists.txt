cmake_minimum_required(VERSION 3.24)
project(Playground)

set(CMAKE_CXX_STANDARD 17)

add_executable(Playground
        graphs/graph.cpp algo_and_ds/main.cpp algo_and_ds/BinarySearchTree.hpp algo_and_ds/Heap.hpp algo_and_ds/SegmentTree.hpp lilutils.hpp undefinedbehavior.cpp cloneable.hpp iterator.hpp)
