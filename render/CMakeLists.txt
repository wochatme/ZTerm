project(render)

file(GLOB LIBTRENDER_SRC 
	"*.cpp" 
)

add_library(${PROJECT_NAME} ${LIBTRENDER_SRC})

set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")  

target_include_directories(${PROJECT_NAME} PUBLIC 
	${PROJECT_SOURCE_DIR} 
	)

# Specify the precompiled header(s)
set(PRECOMPILE_HEADER_FILES pch.h)
target_precompile_headers(${PROJECT_NAME} PRIVATE ${PRECOMPILE_HEADER_FILES})

