cmake_minimum_required(VERSION 2.7)
project("midisurf" C)

add_definitions(-DUNIX)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

add_executable(midisurf src/midisurf.c src/midi.c src/sound.c src/graphics.c
                        src/io.c src/menu.c src/bitmap.c)
target_link_libraries(midisurf m)
