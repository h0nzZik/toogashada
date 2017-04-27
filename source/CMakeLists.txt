cmake_minimum_required(VERSION 3.0)
project(toogashada)

find_package(Boost COMPONENTS system program_options REQUIRED)
find_package(SDL2 COMPONENTS ttf gfx REQUIRED)
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

## Common server/client library

add_library(ToogashadaCommon
	./common/Connection.cpp
	./common/Message.cpp
	./common/TwoDimensionalObject.cpp
	./common/components/EntityID.cpp
	./common/ServerMessage.cpp
	./common/ClientMessage.cpp
)

target_include_directories(ToogashadaCommon
	PUBLIC
    	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_include_directories(ToogashadaCommon
	SYSTEM PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/third_party/cereal/include
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/third_party/EntityPlus
)

#add_library(Geometry
#
#)

## Server

add_library(server
	./server/ConnectionToClient.cpp
	./server/Server.cpp
	./server/GameModel.cpp
)

target_link_libraries(server
	ToogashadaCommon
	${Boost_LIBRARIES}
	${CMAKE_THREAD_LIBS_INIT}
)

add_executable(server_main
	./server/server_main.cpp
)

target_link_libraries(server_main
	server
)

## Client

add_executable(client
	./client/client_main.cpp
	./client/Client.cpp
	./client/ClientController.cpp
	./client/ConnectionToServer.cpp
	./client/RemoteServerWrapper.cpp
	./client/ClientGui.cpp
        client/SDLDeleter.h client/DrawProp.h)

target_include_directories(client PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(client
	ToogashadaCommon
	${Boost_LIBRARIES}
	${SDL2_LIBRARIES}
	${CMAKE_THREAD_LIBS_INIT}
	SDL2_gfx
	${Boost_LIBRARIES}
	#	SDL2_ttf
)

# Tests
add_executable(test_server
	./test/doctest_main.cpp
	./test/test_server.cpp
)

target_link_libraries(test_server
	server
)
