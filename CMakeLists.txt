cmake_minimum_required(VERSION 3.12)
project(RawTCP C)

set(CLIENT "RawTCP")
set(SERVER "RawTCPServer")

set(CMAKE_C_STANDARD 99)

add_executable(${CLIENT} main.c includes.h functions.h functions.c)
add_executable(${SERVER} server/server.c includes.h functions.h functions.c)
add_custom_command( TARGET RawTCP POST_BUILD
                    COMMAND sudo chown root:root ${CLIENT}
                    COMMAND sudo chmod u+s ${CLIENT})
add_custom_command( TARGET RawTCPServer POST_BUILD
        COMMAND sudo chown root:root ${SERVER}
        COMMAND sudo chmod u+s ${SERVER})