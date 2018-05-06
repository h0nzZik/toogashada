cmake_minimum_required(VERSION 2.7)
project(toogashada)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(cotire)
include(ExternalProject)

# Clang-tidy integration
#if (NOT ${CMAKE_VERSION} VERSION_LESS "3.6")
#	if(NOT CMAKE_CXX_CLANG_TIDY)
#		SET(CMAKE_CXX_CLANG_TIDY "clang-tidy")
#	endif()
#endif()

# Include what you use integration
#if(NOT ${CMAKE_VERSION} VERSION_LESS "3.4")
#	if(NOT CMAKE_CXX_INCLUDE_WHAT_YOU_USE)
#		SET(CMAKE_CXX_INCLUDE_WHAT_YOU_USE "include-what-you-use")
#	endif()
#endif()

find_package(Boost COMPONENTS system program_options REQUIRED)
find_package(PkgConfig)
pkg_search_module(SDL2 sdl2)
pkg_search_module(SDL2_TTF SDL2_ttf)

if(NOT "${SDL2_FOUND}")
	message("looking for SDL2")
	find_package(SDL2 REQUIRED)
endif()
if(NOT "${SDL2_TTF_FOUND}")
	find_package(SDL2_ttf REQUIRED)
endif()

find_package(Threads REQUIRED)

# workaround for the trailing whitespace problem
string(STRIP ${SDL2_LIBRARIES} SDL2_LIBRARIES)

SET(MY_CXX_FLAGS "-std=c++14 -Wall -Wextra -pedantic -Weffc++ -Wold-style-cast")
list(APPEND CMAKE_CXX_FLAGS ${MY_CXX_FLAGS})

# TODO hanle warning flags separately,
# based on used compiler version
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
#	-std=c++14 \
#	-Wall -Wextra -pedantic \
#	-Weffc++ \
#	-Wold-style-cast -Weffc++ \
#	-Wno-error=parentheses \
#	-Wno-error=unused-variable \
#	-Wno-error=unused-parameter \
#")
#	-Werror \
#	-Wno-error=unused-private-field \
#")


# SDL2_gfx

ExternalProject_Add(project_sdl2_gfx
	URL http://www.ferzkopp.net/Software/SDL2_gfx/SDL2_gfx-1.0.3.tar.gz
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/SDL2_gfx
    CONFIGURE_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/third_party/SDL2_gfx/configure --prefix=<INSTALL_DIR>
    BUILD_COMMAND ${MAKE}
)

add_library(libsdl2_gfx STATIC IMPORTED)
ExternalProject_Get_Property(project_sdl2_gfx install_dir)
set_property(TARGET libsdl2_gfx PROPERTY IMPORTED_LOCATION ${install_dir}/lib/libSDL2_gfx.a)
add_dependencies(libsdl2_gfx project_sdl2_gfx)
SET(LIBSDL2_GFX_INCLUDE_PATH "${install_dir}/include/SDL2")



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
	./common/components/Shape.cpp
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

cotire(ToogashadaCommon)

## Server

add_library(server
	./server/ConnectionToClient.cpp
	./server/Server.cpp
	./server/GameModel.cpp
	./server/Connections.cpp
)

target_link_libraries(server
	ToogashadaCommon
	${Boost_LIBRARIES}
	${CMAKE_THREAD_LIBS_INIT}
)

cotire(server)

add_executable(server_main
	./server/server_main.cpp
)

target_link_libraries(server_main
	server
)

cotire(server_main)

## Client

add_library(client
	./client/Client.cpp
	./client/ClientController.cpp
	./client/ConnectionToServer.cpp
	./client/ClientGui.cpp
	./client/SDLWrapper.cpp
)

target_include_directories(client PRIVATE
	${SDL2_INCLUDE_DIRS}
	${LIBSDL2_GFX_INCLUDE_PATH}
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/third_party/ColorConverter
)

target_link_libraries(client
	ToogashadaCommon
	libsdl2_gfx
	${Boost_LIBRARIES}
	${CMAKE_THREAD_LIBS_INIT}
	#	${SDL2_GFX_LIBRARIES}
	${SDL2_LIBRARIES}
	${SDL2_TTF_LIBRARIES}
)

cotire(client)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/media/Biotypc.ttf ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

add_executable(client_main
	./client/client_main.cpp
        )

target_link_libraries(client_main
	client
)

cotire(client_main)

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
