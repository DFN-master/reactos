spec2def(d2d1.dll d2d1.spec ADD_IMPORTLIB)

list(APPEND SOURCE
    d2d1_main.c
    ${CMAKE_CURRENT_BINARY_DIR}/d2d1.def)

add_library(d2d1 MODULE ${SOURCE})
set_module_type(d2d1 win32dll)
target_link_libraries(d2d1 wine)
add_importlibs(d2d1 msvcrt kernel32 ntdll)
add_cd_file(TARGET d2d1 DESTINATION reactos/system32 FOR all)
