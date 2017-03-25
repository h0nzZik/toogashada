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
	-Wno-error=unused-lambda-capture \
")

include_directories(${Boost_INCLUDE_DIRS})

## Common server/client library

add_library(ToogashadaCommon
	./src/Connection.cpp
	./src/Messages.cpp
)

## Server

add_executable(server
	./src/server.cpp
	./src/server_networking.cpp
	./src/ConnectionToClient.cpp
)

target_link_libraries(server
	ToogashadaCommon
	${Boost_LIBRARIES}
	${CMAKE_THREAD_LIBS_INIT}
)

## Client

add_executable(client
	./src/client.cpp
	./src/ConnectionToServer.cpp
)

target_link_libraries(client
	ToogashadaCommon
	${Boost_LIBRARIES}
	${SDL2_LIBRARIES}
	${CMAKE_THREAD_LIBS_INIT}
	#	SDL2_ttf
)

