#!/bin/sh
echo `grep -E '^[[:blank:]]+VERSION' CMakeLists.txt | grep -Eo  '[[:digit:]]+\.[[:digit:]]+\.[[:digit:]]+'`
