cmake_minimum_required(VERSION 3.0)
project(toogashada)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
	-std=c++1z \
	-Wall -Wextra -pedantic -Werror \
	-Wold-style-cast -Weffc++ \
	-Wno-error=parentheses \
	-Wno-error=unused-variable \
	-Wno-error=unused-parameter \
")

add_executable(client src/client.cpp)
add_executable(server src/server.cpp)
