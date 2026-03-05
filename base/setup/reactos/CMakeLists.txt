
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib
#    ${REACTOS_SOURCE_DIR}/base/setup/lib
    ${REACTOS_SOURCE_DIR}/sdk/include/reactos/libs/zlib
    ${REACTOS_SOURCE_DIR}/sdk/include/reactos/drivers)

list(APPEND SOURCE
    spapisup/fileqsup.c
    spapisup/infsupp.c
    drivepage.c
    reactos.c
    treelist.c
    modern_ui.c
    modern_pages.c
    modern_admin_page.c
    reactos.h
    modern_setup.h
    modern_resource.h)

file(GLOB reactos_rc_deps res/*.* modern_dialogs.rc)
add_rc_deps(reactos.rc ${reactos_rc_deps})
add_executable(reactos ${SOURCE} reactos.rc modern_dialogs.rc)

add_pch(reactos reactos.h SOURCE)
set_module_type(reactos win32gui UNICODE)
target_link_libraries(reactos uuid)
target_link_libraries(reactos zlib_solo) ## We use USETUP's cabinet implementation
add_delay_importlibs(reactos setuplib)
add_importlibs(reactos advapi32 gdi32 user32 comctl32 shlwapi setupapi uxtheme dwmapi msimg32 msvcrt kernel32 ntdll)
add_cd_file(TARGET reactos DESTINATION reactos NO_CAB FOR bootcd)
