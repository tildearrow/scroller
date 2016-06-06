# scroller
scroller is a simple application which creates a window displaying scrolled text. it is supposed to be run from a terminal/command line, and often, piped.

# Warning on Windows
there is a bug if a width greater than 999 and a height greater than 99 is used.

because of no reason (yes, because i don't find the reason), a heap may be corrupted (parameters: 0x00007FFA366BDD50). if the heap does get corrupted, re-run until it does not say anything, or merely debug and continue on Visual Studio.

## Dependencies
- SDL2, SDL_ttf and SDL_image
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
### Linux and OS X
open a terminal and
```
./scroller [options] <font> [font...]
```
then feed standard input.

### Windows
open a command prompt, and
```
scroller.exe [options] <font> [font...]
```
then write in the command prompt.
