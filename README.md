# librg example
This is an project example of using old version of librg inside CMake generated project. Bundled with SDL2 to showcase some of the lib features.

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
