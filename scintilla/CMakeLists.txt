project(scintilla)

file(GLOB LIBSCINTILLA_SRC 
	"src/*.cxx" 
	"win32/*.cxx" 
	"win32/*.rc"
)

add_library(${PROJECT_NAME} ${LIBSCINTILLA_SRC})

set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")  

target_include_directories(${PROJECT_NAME} PUBLIC 
	${PROJECT_SOURCE_DIR} 
	${PROJECT_SOURCE_DIR}/src 
	${PROJECT_SOURCE_DIR}/win32
	${PROJECT_SOURCE_DIR}/include
	)
