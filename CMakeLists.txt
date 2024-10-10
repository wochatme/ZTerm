cmake_minimum_required(VERSION 3.20)

project(ZTerm)

set(CMAKE_CXX_STANDARD 20)
add_subdirectory(win)
add_subdirectory(term)
add_subdirectory(render)
add_subdirectory(scintilla)
add_subdirectory(curl)
add_subdirectory(zlib)
add_subdirectory(zt)
add_subdirectory(pcre2)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT zterm-win64)

