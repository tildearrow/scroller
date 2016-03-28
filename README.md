# scroller
scrolltext fed by stdin

## Dependencies
SDL2 and SDL_ttf

CMake for building
## Compiling
### Linux
warning, you'll have to either KILL, QUIT or TERM scroller, until i fix it
```
mkdir build; cd build; cmake ..; make; ./scroller <font> <size> <scroller width>
```

### OS X
warning, you'll have to quit scroller unexpectedly, until i fix it
also, install dependencies using brew before compiling
```
mkdir build; cd build; cmake ..; make; ./scroller <font> <size> <scroller width>
```

### Windows
install Visual Studio 2015, and then clone and build this project

the default run uses the Segoe UI font with a scroller width of 1024

otherwise, use
```
scroller.exe <font> <size> <scroller width>
```
