
add_definitions(
    -DISOHYBRID_C_STANDALONE
    -DREACTOS_ISOHYBRID_EFI_MAC_SUPPORT)

add_host_tool(isohybrid
    isohybrid.c
    reactos_support_code.c
    ../port/getopt.c
    ../port/getopt1.c)

if(MSVC)
    target_link_libraries(isohybrid rpcrt4)
endif()
