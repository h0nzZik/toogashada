# Instructions to build and run the program on Linux platform.
(Just follow CMake and linker errors)

## Requirements:
* CMake, Clang >= 3.9
* project source code - provided 
* EntityPlus library - provided as a git submodule
* Cereal library - provided as a git submodule
* SDL2 library - https://www.libsdl.org/download-2.0.php
* SDL2_gfx library - http://www.ferzkopp.net/wordpress/2016/01/02/sdl_gfx-sdl2_gfx/
* Boost.Asio - not provided
* Boost.Variant - not provided
* Biotypc font - provided

## Installation:
1, Clone the git repo.
2, Run `git submodule update --init --recursive` to initialize submodules (EntityPlus, Cereal).
3, Install (development versions if available of) SDL2, SDL2_gfx and Boost libraries. You can use your favourite package manager.
4, Create arbitrary build directory and use cmake to compile the project.
4, Copy Biotypc.ttf file to your build folder.

## Running the program:
First you need to run the server (cmake target: server_main), which will run server on localhost:2061.
Then clients (cmake target: client_main) can connect. Client accepts parameters --ip(-i), --port(-p), --player-nane(-n) and optionally --player-team(-t), --window-width(-w) and --window-height(-h).
Example for connecting to above mentioned server:
```
./client_main -i localhost -p 2061 -n Lukas -t Developers -w 500 -h 500
```
In case of wrong parameters inputed, the client will print help.