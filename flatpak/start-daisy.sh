#!/bin/bash
APP_ID="dk.ku.daisy"
DATA_DIR="${XDG_DATA_HOME}" # Flatpak always sets XDG_DATA_HOME

# Create user data dir if missing
mkdir -p "$DATA_DIR"

# Copy bundled defaults if not already present
if [ ! -d "$DATA_DIR/lib" ]; then
    echo "Installing lib and sample"
    cp -r /app/share/daisy/lib "$DATA_DIR"
    cp -r /app/share/daisy/sample "$DATA_DIR"
fi

if [[ "$1" == "--pip-install" ]]; then
    python -m ensurepip
    if [ $# -ge 2 ]; then
        python -m pip install "${@:2}"
    fi
elif [[ "$1" == "--pip-uninstall" ]]; then
    if [ $# -ge 2 ]; then
        python -m pip uninstall "${@:2}"
    fi
else
    # Ensure paths are absolute
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
