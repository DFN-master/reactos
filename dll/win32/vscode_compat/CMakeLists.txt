
spec2def(vscode_compat.dll vscode_compat.spec ADD_IMPORTLIB)

include_directories(
    ${REACTOS_SOURCE_DIR}/sdk/include/reactos/wine)

list(APPEND SOURCE
    appcontainer.c)

add_library(vscode_compat MODULE
    ${SOURCE}
    ${CMAKE_CURRENT_BINARY_DIR}/vscode_compat.def)

set_module_type(vscode_compat win32dll)
target_link_libraries(vscode_compat wine uuid)
add_importlibs(vscode_compat msvcrt kernel32 ntdll)
add_cd_file(TARGET vscode_compat DESTINATION reactos/system32 FOR all)
