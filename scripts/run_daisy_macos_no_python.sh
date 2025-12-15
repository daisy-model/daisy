#!/bin/zsh
# Run Daisy with DAISYHOME set
reldir=$(dirname -- "$0")
absdir=$(cd -- "$reldir" && pwd)
daisy="$absdir/daisy-bin"

# Check if user already set DAISYHOME, otherwise set it relative to the script
if [[ -v DAISYHOME ]]; then
    daisyhome="$DAISYHOME"
else
    daisyhome="$absdir/.."
fi

if [[ "$1" == "--version" ]]; then
    "$daisy" -v 2>&1 | head -n 1 && rm daisy.log
    echo "No python"
else
    DAISYHOME="$daisyhome" "$daisy" "$@"
fi
