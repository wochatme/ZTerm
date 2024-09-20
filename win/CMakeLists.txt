# the build for Windows X64
project(zterm-win64 CXX)

add_definitions(-D_UNICODE -DUNICODE -DNOMINMAX)

add_executable(${PROJECT_NAME} WIN32 
	App.cpp
	pch.cpp
	Render.cpp
	Terminal.cpp
	Settings.cpp
	Network.cpp
	App.rc
	)

# Specify the precompiled header(s)
set(PRECOMPILE_HEADER_FILES pch.h)
target_precompile_headers(${PROJECT_NAME} PRIVATE ${PRECOMPILE_HEADER_FILES})

set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

target_include_directories(${PROJECT_NAME} PRIVATE 
	${PROJECT_SOURCE_DIR} 
	${PROJECT_SOURCE_DIR}/wtl 
	${CMAKE_SOURCE_DIR}
	)

target_link_libraries(${PROJECT_NAME} scintilla)
target_link_libraries(${PROJECT_NAME} zlibstatic)
target_link_libraries(${PROJECT_NAME} libcurl_static)

