#include "windows/windows_util.h"

std::basic_string<TCHAR> get_exe_path() {
    return GetStringFromWindowsApi<TCHAR>( []( TCHAR* buffer, int size ) {
        return GetModuleFileName( NULL, buffer, size );
    });
    
}

std::basic_string<TCHAR> get_daisy_home_from_exe_path() {
    auto exe_path = get_exe_path();
    auto n = exe_path.rfind("\\bin\\daisy.exe");
    if (n == std::basic_string<TCHAR>::npos) {
        return "";
    }
    return exe_path.substr(0, n);
}