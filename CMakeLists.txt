cmake_minimum_required(VERSION 3.18)

project(easystroke LANGUAGES C CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -std=c++11")

file(GLOB SOURCES
    *.c *.cc *.h *.cpp
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(MODULES REQUIRED
    x11 xi xext xfixes xtst
    gtkmm-3.0
    dbus-glib-1
)

find_package(Boost REQUIRED COMPONENTS serialization filesystem)

add_executable(easystroke
    ${SOURCES}
    ${CMAKE_CURRENT_BINARY_DIR}/gui.c
    ${CMAKE_CURRENT_BINARY_DIR}/cellrenderertextish.c
)

target_link_libraries(easystroke
    PRIVATE
    ${MODULES_LIBRARIES}
    ${Boost_LIBRARIES}
)
target_include_directories(easystroke
    SYSTEM PRIVATE
    ${MODULES_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)

target_compile_options(easystroke
    PRIVATE
    -DLOCALEDIR="${CMAKE_INSTALL_PREFIX}/share/locale/"
    -DGETTEXT_PACKAGE="easystroke"
    -I${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_command(OUTPUT gui.c
    COMMAND echo 'const char *gui_buffer = \"\\' > gui.c
    COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/gui.glade
        | sed -s 's/\"GtkWindow\"/\"GtkApplicationWindow\"/'
        | sed -s 's/\"/\\\\\"/g'
        | sed -s 's/$$/\\\\n\\\\/g'
        >> gui.c
    COMMAND echo '\"\;' >> gui.c
    DEPENDS gui.glade
)

add_custom_command(OUTPUT cellrenderertextish.c
    COMMAND valac -c ${CMAKE_CURRENT_SOURCE_DIR}/cellrenderertextish.vala --pkg gtk+-3.0 -C -H cellrenderertextish.h
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/cellrenderertextish.vala
)

install(
    TARGETS easystroke
    DESTINATION bin/
)
