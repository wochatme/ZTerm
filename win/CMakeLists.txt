# the build for Windows X64
project(zterm-win64 CXX)

add_definitions(-D_UNICODE -DUNICODE -DNOMINMAX)

add_executable(${PROJECT_NAME} WIN32 
	ZTerm.cpp
	stdafx.cpp
	PuTTY.cpp
	Config.cpp
	NetWork.cpp
	ZTerm.rc
	)

# Specify the precompiled header(s)
set(PRECOMPILE_HEADER_FILES stdafx.h)
target_precompile_headers(${PROJECT_NAME} PRIVATE ${PRECOMPILE_HEADER_FILES})

# set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

target_include_directories(${PROJECT_NAME} PRIVATE 
	${PROJECT_SOURCE_DIR} 
	${PROJECT_SOURCE_DIR}/wtl 
	${CMAKE_SOURCE_DIR}/curl
	${CMAKE_SOURCE_DIR}/putty
	${CMAKE_SOURCE_DIR}/putty/terminal
	${CMAKE_SOURCE_DIR}/putty/windows
	)

target_link_libraries(${PROJECT_NAME} libwt)
target_link_libraries(${PROJECT_NAME} zlibstatic)
target_link_libraries(${PROJECT_NAME} libcurl_static)
target_link_libraries(${PROJECT_NAME} scintilla)
# The library from PuTTY codebase
target_link_libraries(${PROJECT_NAME} guiterminal)
target_link_libraries(${PROJECT_NAME} eventloop)
target_link_libraries(${PROJECT_NAME} network)
target_link_libraries(${PROJECT_NAME} settings)
target_link_libraries(${PROJECT_NAME} utils)
target_link_libraries(${PROJECT_NAME} guimisc)
target_link_libraries(${PROJECT_NAME} libterm)

