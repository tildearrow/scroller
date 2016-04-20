# scroller
scroller is a simple application which creates a window displaying scrolled text. it is supposed to be run from a terminal/command line, and often, piped.

## Dependencies
- SDL2 and SDL_ttf
- CMake for building

## Compiling
### Linux
```
mkdir build; cd build; cmake ..; make;
```

### OS X
warning: install dependencies using brew before compiling or running
```
mkdir build; cd build; cmake ..; make
```

### Windows
install Visual Studio 2015, and then clone and build this project

the default run uses the Segoe UI font with a scroller width of 1024

## Running
### Linux
open a terminal and
```
./scroller <font> <size> <scroller width>
```
then feed standard input.

### OS X
open a terminal and
```
./scroller <font> <size> <scroller width>
```
then feed standard input.

### Windows
open a command prompt, and
```
scroller.exe <font> <size> <scroller width>
```
then write in the command prompt.
