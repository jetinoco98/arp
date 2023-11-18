cmake_minimum_required(VERSION 3.2)

project("Week 8 homework")

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

find_package(Curses REQUIRED)

include_directories("./headers")
set(WRAPSRC utility/wrapfunc.c)

add_executable(master master.c ${WRAPSRC})
add_executable(reader reader.c ${WRAPSRC})
add_executable(server server.c ${WRAPSRC})
add_executable(writer writer.c ${WRAPSRC})

target_link_libraries(master ${CURSES_LIBRARIES})
target_link_libraries(reader ${CURSES_LIBRARIES})
target_link_libraries(server ${CURSES_LIBRARIES})
target_link_libraries(writer ${CURSES_LIBRARIES})
