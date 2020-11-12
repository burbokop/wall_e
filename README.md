# wall_e
set of compilation tools

## tools present
lexical deconstruction</br>
gramatical processing</br>
variant (like qt variant)</br>
other tools for translation</br>

## example
https://github.com/burbokop/wall_e_demo

## adding wall_e to your c++ project

1. `git submodule add https://github.com/burbokop/wall_e.git`</br>
2. in cmake: 
```
include(${CMAKE_CURRENT_SOURCE_DIR}/wall_e/wall_e.cmake)
target_link_libraries(your_executable_or_lib
    wall_e
)

```

## dependencies
    curses 
    ncurses    

linux installation: `sudo apt install libcurses-ocaml-dev`
