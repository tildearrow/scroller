# scroller
scrolltext fed by stdin

## Dependencies
SDL2 and SDL_ttf

CMake for building
## Compiling
### Linux
```
mkdir build; cd build; cmake ..; make; ./scroller <font> <size> <scroller width>
```

### OS X
warning, this seems to work but the program freezes on launch
```
mkdir build; cd build; cmake ..; make; ./scroller <font> <size> <scroller width>
```

### Windows
no, sorry. (until i get to fix it, or maybe try using MinGW?)
