# ReGuider Framework

This framework offers a complete suite to host multiplayer games and applications.

## Pre-install
Before starting, make sure you have installed [CMake](https://cmake.org/). It will be used to generate make files for your system.
Also make sure you have installed a working compiler for your system.

## Installation

```sh
$ git clone --recursive git@gitlab.com:reguider/reguider.git
$ mkdir -p build && cd build
$ cmake ..
```

## Building

Go inside the build folder, there you can find either your generated project files.

Unix:

```sh
cd build
make -j<numcores>
```

Windows:
If you are using Visual Studio, proper project/solution files should be generated for your visual studio.
Run build, and enjoy.

## Used dependencies
* [libuv/libuv](https://github.com/libuv/libuv) 1.9.0 via cmake interface [Inlife/libuv-cmake](https://github.com/Inlife/libuv-cmake)
* [Inlife/RakNet](https://github.com/Inlife/RakNet) 4.081 (slightly modified version)
* [inlife/squirrel-lang](https://github.com/inlife/squirrel-lang) 3.0 stable with cmake interface

## Compilation tested on
* macos 10.12.2 with g++ 4.2.1
* ubuntu 16.04 with g++
* windows 10 with msvc 14.0

## Bugs
* windows 10 mingw 5.3.0, problems with RakNet
