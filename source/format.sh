#!/bin/sh
clang-format -style=llvm -i ./common/*.h ./common/*.cpp 
clang-format -style=llvm -i ./server/*.h ./server/*.cpp 
clang-format -style=llvm -i ./client/*.h ./client/*.cpp 

