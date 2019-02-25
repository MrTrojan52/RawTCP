cmake_minimum_required(VERSION 3.12)
project(RawTCP C)

set(TARGET "RawTCP")

set(CMAKE_C_STANDARD 99)

add_executable(${TARGET} main.c)

add_custom_command( TARGET RawTCP POST_BUILD
                    COMMAND sudo chown root:root ${TARGET}
                    COMMAND sudo chmod u+s ${TARGET})