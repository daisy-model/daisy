rem set PATH=C:\msys64\ucrt64\bin;C:\msys64\usr\bin;%PATH%
rem cd C:\werkmap\PHISHIS\coupling_daisy\src\daisy-7.0.7\daisy-7.0.7
rem cmake --preset mingw-gcc-native -B build/release
rem cmake --build build/release

set PATH=C:\msys64\ucrt64\bin;C:\msys64\usr\bin;%PATH%
cd c:\src\daisy

rem configure (paths defined in CMakeUserPresets.json)
cmake --preset mingw-gcc-native-local

rem build the core
cmake --build build/release --target core

rem rebuild only the .pyd target
cmake --build build/release --target daisy_bmi

pause