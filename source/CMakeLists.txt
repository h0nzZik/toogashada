cmake_minimum_required(VERSION 3.0)
project(toogashada)

find_package(Boost COMPONENTS system REQUIRED)
find_package(SDL2 COMPONENTS ttf REQUIRED)
find_package(Threads REQUIRED)

# workaround for the trailing whitespace problem
string(STRIP ${SDL2_LIBRARIES} SDL2_LIBRARIES)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
	-std=c++14 \
	-Wall -Wextra -pedantic -Werror \
	-Wold-style-cast -Weffc++ \
	-Wno-error=parentheses \
	-Wno-error=unused-variable \
	-Wno-error=unused-parameter \
	-Wno-error=unused-private-field \
")

include_directories(${Boost_INCLUDE_DIRS})

## TODO find package Threads

## Server

add_executable(server
	src/server.cpp
	src/server_networking.cpp
)

target_link_libraries(server ${Boost_LIBRARIES})

## Client

add_executable(client
	./src/client.cpp
	./src/ConnectionToServer.cpp
)

target_link_libraries(client
	${Boost_LIBRARIES}
	${SDL2_LIBRARIES}
	${CMAKE_THREAD_LIBS_INIT}
	#	SDL2_ttf
)

