# Mosquitto的编译

## 下载：
```
git clone https://github.com/eclipse/mosquitto.git
```

## 编译
假设下载的源码在d:\github\mosquitto

随便选择一个目录，譬如d:\build\mosquitto
在编译之前，修改CMakeLists.txt

我只想要没有任何附加功能的静态库，所以把如下编译开关关闭掉：
```
option(WITH_TLS
	"Include SSL/TLS support?" OFF)
option(WITH_TLS_PSK
	"Include TLS-PSK support (requires WITH_TLS)?" OFF)
option(WITH_EC
	"Include Elliptic Curve support (requires WITH_TLS)?" OFF)

option(WITH_UNIX_SOCKETS "Include Unix Domain Socket support?" OFF)

option(WITH_THREADING "Include client library threading support?" OFF)

option(WITH_DLT "Include DLT support?" OFF)

option(WITH_CJSON "Build with cJSON support (required for dynamic security plugin and useful for mosquitto_sub)?" OFF)

option(WITH_CLIENTS "Build clients?" OFF)
option(WITH_BROKER "Build broker?" OFF)
option(WITH_APPS "Build apps?" OFF)
option(WITH_PLUGINS "Build plugins?" OFF)
option(DOCUMENTATION "Build documentation?" OFF)
option(WITH_SRV "Include SRV lookup support?" OFF)
```

把如下开关打开：
```
option(WITH_STATIC_LIBRARIES "Build static versions of the libmosquitto/pp libraries?" ON)

```

修改lib子目录下的CMakeLists.txt

关闭cpp的库，cpp就一个源文件，自己抄袭就行了。
```
option(WITH_LIB_CPP "Build C++ library?" OFF)

```

在静态库部分，加入如下代码：
```
if (WITH_STATIC_LIBRARIES)
	add_library(libmosquitto_static STATIC ${C_SRC})

	set_property(TARGET libmosquitto_static PROPERTY                 ######## add by myself
	  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")  ######## add by myself
  	
	if (WITH_PIC)
		set_target_properties(libmosquitto_static PROPERTIES
			POSITION_INDEPENDENT_CODE 1
		)
	endif (WITH_PIC)

	target_link_libraries(libmosquitto_static ${LIBRARIES})

	set_target_properties(libmosquitto_static PROPERTIES
		OUTPUT_NAME mosquitto_static
		VERSION ${VERSION}
	)

	target_compile_definitions(libmosquitto_static PUBLIC "LIBMOSQUITTO_STATIC")
	install(TARGETS libmosquitto_static ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")
endif (WITH_STATIC_LIBRARIES)

```

然后在d:\build\mosquitto目录下执行
```
cmake -G "Visual Studio 17" d:\github\mosquitto
```

就编译出静态库和动态库。

