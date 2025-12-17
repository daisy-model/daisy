#!/bin/zsh
# Run Daisy with PYTHONHOME and DAISYHOME set
reldir=$(dirname -- "$0")
absdir=$(cd -- "$reldir" && pwd)
sampledir=$(cd -- "$reldir/../sample" && pwd)
daisy="$absdir/daisy-bin"
pythonhome="$absdir/../python"

# Check if user already set DAISYHOME, otherwise set it relative to the script
if [[ -v DAISYHOME ]]; then
    daisyhome="$DAISYHOME"
else
    daisyhome="$absdir/.."
fi

if [[ "$1" == "--info" ]]; then
    # Print Daisy and python info
    PYTHONHOME="$pythonhome" "$daisy" -v 2>&1 | head -n 1 && rm daisy.log
    "$absdir"/python --version
    echo "Sample dir: $sampledir"

elif [[ "$1" == "--pip" ]]; then
    # Manage python environment
    "$absdir/python" -m ensurepip
    if [ $# -ge 2 ]; then
	if [[ "$2" == "install" ]]; then
            exec "$absdir/python" -m pip "${@:2}" --no-warn-script-location
	else
	    exec "$absdir/python" -m pip "${@:2}"
	fi
    fi

elif [[ "$1" == "--python" ]]; then
    # Run the python interpreter
    exec "$absdir/python" "${@:2}"

else
    PYTHONHOME="$pythonhome" DAISYHOME="$daisyhome" exec "$daisy" "$@"
fi
