#!/bin/sh
# Run Daisy with PYTHONHOME set
# This requries that there is a symlink in <daisydir>/bin/lib/python to a python installation
# If it is not present, then we prompt the user for a path
# Once we have the path we create the symlink and then run daisy

reldir=$(dirname -- "${BASH_SOURCE[0]}")
absdir=$(cd -- "$reldir" && pwd)
daisy="$absdir"/daisy-bin
libdir=lib

# This script is intended for use with an installed daisy
# If you want to run daisy directly from the build directory, you should uncomment the following line
# libdir=bin/lib

pythonhome="$absdir"/"$libdir"/python

while [[ ! -f "$pythonhome"/bin/python ]]
do
    read -e -p "Please provide path to python root directory: " python_dir
    if [[ -f "$python_dir/bin/python" &&  -d "$python_dir/lib" ]]
    then
	cd ${absdir}/"$libdir" && rm -f python && ln -s ${python_dir} python
	cd ${absdir}
    else
	echo "Could not locate python binary and/or library"
    fi
done
PYTHONHOME="$pythonhome" "$daisy" "$@"
