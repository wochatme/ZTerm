project(libzt)

file(GLOB LIBZT_SRC 
	"zt_mempool.c"
	"zt_sha256.c"
	"zt_unicode.c"
	)

add_library(${PROJECT_NAME} ${LIBZT_SRC})

set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

target_include_directories(${PROJECT_NAME} 
    PUBLIC ${PROJECT_SOURCE_DIR} 
           ${CMAKE_SOURCE_DIR}
           )
           
