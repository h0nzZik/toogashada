cmake_minimum_required(VERSION 3.0)
project(toogashada)

find_package(Boost COMPONENTS system REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
	-std=c++14 \
	-Wall -Wextra -pedantic -Werror \
	-Wold-style-cast -Weffc++ \
	-Wno-error=parentheses \
	-Wno-error=unused-variable \
	-Wno-error=unused-parameter \
")

include_directories(${Boost_INCLUDE_DIRS})

add_executable(client src/client.cpp)
add_executable(server
	src/server.cpp
	src/server_networking.cpp
)

target_link_libraries(client ${Boost_LIBRARIES})
target_link_libraries(server ${Boost_LIBRARIES})
