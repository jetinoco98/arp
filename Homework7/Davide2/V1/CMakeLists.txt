cmake_minimum_required(VERSION 3.2)

project("Week 7 homework")

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

find_package(Curses REQUIRED)

include_directories("./headers")
set(WRAPSRC utility/wrapfunc.c)

add_executable(master master.c ${WRAPSRC})
add_executable(watchdog watchdog.c ${WRAPSRC})
add_executable(process process.c ${WRAPSRC})

target_link_libraries(master ${CURSES_LIBRARIES})
target_link_libraries(process ${CURSES_LIBRARIES})
target_link_libraries(watchdog ${CURSES_LIBRARIES})

#target_compile_options(master PRIVATE -Wall -Wextra -Wpedantic -Werror)
#target_compile_options(watchdog PRIVATE -Wall -Wextra -Wpedantic -Werror)
#target_compile_options(process PRIVATE -Wall -Wextra -Wpedantic -Werror)
