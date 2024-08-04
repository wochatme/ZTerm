project(libwt)

file(GLOB LIBWT_SRC 
	"wt_mempool.c"
	"wt_hash.c"
	"wt_base64.c"
	"wt_aes256.c"
	"wt_chacha20.c"
	"wt_sha256.c"
	"wt_utils.c"
	"wt_unicode.c"
	"wt_render.c"
	)

add_library(${PROJECT_NAME} ${LIBWT_SRC})

# set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

target_include_directories(${PROJECT_NAME} 
    PUBLIC ${PROJECT_SOURCE_DIR} 
           ${CMAKE_SOURCE_DIR}
           )
           
