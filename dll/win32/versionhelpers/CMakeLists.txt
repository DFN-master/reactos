
spec2def(versionhelpers.dll versionhelpers.spec)

list(APPEND SOURCE
    versionhelpers.c
    ${CMAKE_CURRENT_BINARY_DIR}/versionhelpers.def)

add_library(versionhelpers MODULE ${SOURCE})
set_module_type(versionhelpers win32dll)
target_link_libraries(versionhelpers wine)
add_importlibs(versionhelpers msvcrt kernel32 ntdll)
add_cd_file(TARGET versionhelpers DESTINATION reactos/system32 FOR all)
