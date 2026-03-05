spec2def(msvcp140.dll msvcp140.spec ADD_IMPORTLIB)

include_directories(
    ${REACTOS_SOURCE_DIR}/sdk/include/reactos/wine)

list(APPEND SOURCE
    msvcp140_main.c)

add_library(msvcp140 MODULE
    ${SOURCE}
    ${CMAKE_CURRENT_BINARY_DIR}/msvcp140.def)

set_module_type(msvcp140 win32dll)
target_link_libraries(msvcp140 wine)
add_importlibs(msvcp140 msvcrt kernel32 ntdll)
add_cd_file(TARGET msvcp140 DESTINATION reactos/system32 FOR all)
