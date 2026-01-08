#!/bin/bash
APP_ID="dk.ku.daisy"
DATA_DIR="${XDG_DATA_HOME}" # Flatpak always sets XDG_DATA_HOME

# Create user data dir if missing
mkdir -p "$DATA_DIR"

# Copy bundled defaults if not already present
if [ ! -d "$DATA_DIR/lib" ] || [ ! -d "$DATA_DIR/sample" ]; then
    echo "Installing lib and sample"
    cp -r /app/share/daisy/lib "$DATA_DIR"
    cp -r /app/share/daisy/sample "$DATA_DIR"
fi

if [[ "$1" == "--info" ]]; then
    # Print Daisy and python info
    DAISYHOME="$DATA_DIR" /app/bin/daisy -v 2>&1 | head -n 1 && rm daisy.log
    python --version
    echo "Sample dir: $DATA_DIR/sample"

elif [[ "$1" == "--pip" ]]; then
    # Manage python environment
    python -m ensurepip
    if [ $# -ge 2 ]; then
        exec python -m pip "${@:2}"
    fi

elif [[ "$1" == "--python" ]]; then
    # Run python interpreter
    exec python "${@:2}"

else
    # Run Daisy
    # First ensure paths are absolute
    params=()
    for arg in "$@"; do
        if [ -f "$arg" ]; then
            params+=($(realpath "$arg"))
        else
            params+=("$arg")
        fi
    done
    DAISYHOME="$DATA_DIR" exec /app/bin/daisy "${params[@]}"
fi
