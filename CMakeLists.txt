cmake_minimum_required(VERSION 3.18)

project(easystroke LANGUAGES C CXX)

file(GLOB SOURCES
    *.c *.cc
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(MODULES REQUIRED
    x11 xi xext xfixes xtst
    gtkmm-3.0
    dbus-glib-1
)

find_package(Boost REQUIRED COMPONENTS serialization)

add_executable(easystroke
    ${SOURCES}
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
)
