spec2def(vcruntime140.dll vcruntime140.spec ADD_IMPORTLIB)

include_directories(
    ${REACTOS_SOURCE_DIR}/sdk/include/reactos/wine)

list(APPEND SOURCE
    vcruntime140_main.c)

add_library(vcruntime140 MODULE
    ${SOURCE}
    ${CMAKE_CURRENT_BINARY_DIR}/vcruntime140.def)

set_module_type(vcruntime140 win32dll)
target_link_libraries(vcruntime140 wine)
add_importlibs(vcruntime140 msvcrt kernel32 ntdll)
add_cd_file(TARGET vcruntime140 DESTINATION reactos/system32 FOR all)
