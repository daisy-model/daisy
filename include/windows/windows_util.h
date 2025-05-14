#ifndef DAISY_WINDOWS_UTIL_H
#define DAISY_WINDOWS_UTIL_H
#if defined(_WIN32) || (__CYGWIN__32__)
#include <string>
#include <Windows.h>


// From https://stackoverflow.com/a/54491532
template <typename TChar, typename TStringGetterFunc>
std::basic_string<TChar> GetStringFromWindowsApi( TStringGetterFunc stringGetter, int initialSize = 0 )
{
    if( initialSize <= 0 )
    {
        initialSize = MAX_PATH;
    }

    std::basic_string<TChar> result( initialSize, 0 );
    for(;;)
    {
        auto length = stringGetter( &result[0], result.length() );
        if( length == 0 )
        {
            return std::basic_string<TChar>();
        }

        if( length < result.length() - 1 )
        {
            result.resize( length );
            result.shrink_to_fit();
            return result;
        }

        result.resize( result.length() * 2 );
    }
}

std::basic_string<TCHAR> get_exe_path();
std::basic_string<TCHAR> get_daisy_home_from_exe_path();
#endif
#endif