# PTerm Compile

```
cl /MDd /I.. /I../terminal /c winterm.c
cl /MDd /I.. /c pterm.c
cl /MDd /I.. /c help.c
cl /MDd /I.. /c conpty.c

rc /I.. pterm.rc  -- remove #include "putty-common.rc2" from pterm.rc

link /OUT:pterm2.exe /LIBPATH:c:\build\pterm\windows\pterm2-be-list.dir\Debug /LIBPATH:c:\build\pterm\Debug /LIBPATH:c:\build\pterm\windows\Debug /LIBPATH:c:\build\pterm\windows\pterm2.dir\Debug pterm.obj winterm.obj help.obj conpty.obj guiterminal.lib guimisc.lib eventloop.lib settings.lib network.lib utils.lib user32.lib gdi32.lib Ole32.lib Comdlg32.lib Advapi32.lib Imm32.lib Shell32.lib no-gss.obj no-ca-config.obj no-rand.obj nosshproxy.obj pterm.res be_list.obj

```

```
$ pwd
/mnt/c/gpt/pterm/windows
$ cp window.c winterm.c
$ touch winterm.h
```
in c:\gpt\pterm\windows\CMakeLists.txt, add the below at line 154:
```
  add_executable(pterm2
    winterm.c
    pterm.c
    help.c
    conpty.c
    ${CMAKE_SOURCE_DIR}/stubs/no-gss.c
    ${CMAKE_SOURCE_DIR}/stubs/no-ca-config.c
    ${CMAKE_SOURCE_DIR}/stubs/no-rand.c
    ${CMAKE_SOURCE_DIR}/proxy/nosshproxy.c
    pterm.rc)
  be_list(pterm2 pterm2)
  add_dependencies(pterm2 generated_licence_h)
  target_link_libraries(pterm2
    guiterminal guimisc eventloop settings network utils
    ${platform_libraries})
  set_target_properties(pterm2 PROPERTIES
    WIN32_EXECUTABLE ON
    LINK_FLAGS "${LFLAG_MANIFEST_NO}")
  installed_program(pterm2)
```

winterm.h:
```

```