; vcruntime140.dll - Microsoft Visual C++ 2017+ Runtime
; Minimal stub implementation for ReactOS compatibility

@ cdecl __C_specific_handler(ptr long ptr ptr) msvcrt.__C_specific_handler
@ cdecl __CppXcptFilter(long ptr)
@ cdecl __intrinsic_setjmp(ptr) msvcrt._setjmp
@ cdecl __intrinsic_setjmpex(ptr) msvcrt._setjmp
@ cdecl __std_exception_copy(ptr ptr)
@ cdecl __std_exception_destroy(ptr)
@ cdecl __std_terminate()
@ cdecl __std_type_info_compare(ptr ptr)
@ cdecl __std_type_info_destroy_list(ptr)
@ cdecl __std_type_info_hash(ptr)
@ cdecl __std_type_info_name(ptr ptr)
@ cdecl __telemetry_main_invoke_trigger(ptr)
@ cdecl __telemetry_main_return_trigger(ptr)
@ cdecl __vcrt_GetModuleFileNameW(ptr ptr long)
@ cdecl __vcrt_GetModuleHandleW(wstr)
@ cdecl __vcrt_InitializeCriticalSectionEx(ptr long long)
@ cdecl __vcrt_LoadLibraryExW(wstr ptr long)
@ cdecl _configure_narrow_argv(long)
@ cdecl _configure_wide_argv(long)
@ cdecl _crt_at_quick_exit(ptr)
@ cdecl _crt_atexit(ptr)
@ cdecl _execute_onexit_table(ptr)
@ cdecl _get_initial_narrow_environment()
@ cdecl _get_initial_wide_environment()
@ cdecl _initialize_narrow_environment()
@ cdecl _initialize_onexit_table(ptr)
@ cdecl _initialize_wide_environment()
@ cdecl _register_onexit_function(ptr ptr)
@ cdecl _register_thread_local_exe_atexit_callback(ptr)
@ cdecl _seh_filter_dll(long ptr)
@ cdecl _seh_filter_exe(long ptr)
@ cdecl _set_app_type(long)
@ cdecl _set_new_handler(ptr)
@ cdecl memcpy(ptr ptr long) msvcrt.memcpy
@ cdecl memmove(ptr ptr long) msvcrt.memmove
@ cdecl memset(ptr long long) msvcrt.memset
@ cdecl _purecall() msvcrt._purecall
@ cdecl _set_invalid_parameter_handler(ptr)
