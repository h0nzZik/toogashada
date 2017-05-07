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
	./common/ServerMessage.cpp
	./common/ClientMessage.cpp
	./common/Geometry.cpp
	./common/geometry/Object2D.cpp
	./common/geometry/collision.cpp
	./common/geometry/Polygon.cpp
	./common/components/EntityID.cpp
	./common/components/Shape.cpp)

target_include_directories(ToogashadaCommon
	PUBLIC
    	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_include_directories(ToogashadaCommon
	SYSTEM PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/third_party/cereal/include
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/third_party/EntityPlus
)

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

add_library(client
	./client/Client.cpp
	./client/ClientController.cpp
	./client/ConnectionToServer.cpp
	./client/ClientGui.cpp
)


target_include_directories(client PRIVATE ${SDL2_INCLUDE_DIRS}	)
target_link_libraries(client
	ToogashadaCommon
	${Boost_LIBRARIES}
	${SDL2_LIBRARIES}
	${CMAKE_THREAD_LIBS_INIT}
	SDL2_gfx
	${Boost_LIBRARIES}
	SDL2_ttf
)



add_executable(client_main
	./client/client_main.cpp
        )

target_link_libraries(client_main
	client
)

# Tests
add_executable(tester
	./test/doctest_main.cpp
	./test/test_server.cpp
	./test/test_geometry.cpp
	./test/test_EntityComponentSystem.cpp
)

target_link_libraries(tester
	ToogashadaCommon
	server
	client
)
