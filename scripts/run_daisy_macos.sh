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
    PYTHONHOME="$pythonhome" "$daisy" -v 2>&1 | head -n 1 && rm daisy.log
    "$absdir"/python --version
    echo "Sample dir: $sampledir"
else
    PYTHONHOME="$pythonhome" DAISYHOME="$daisyhome" "$daisy" "$@"
fi
