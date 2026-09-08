#!/bin/bash
APP_ID="dk.ku.daisy"
DATA_DIR="${XDG_DATA_HOME}" # Flatpak always sets XDG_DATA_HOME
DATA_STAMP_FILE="$DATA_DIR/.bundled-data.sha256"
BUNDLED_DATA_STAMP_FILE="/app/share/daisy/.bundled-data.sha256"

# Create user data dir if missing
mkdir -p "$DATA_DIR"

compute_bundled_data_stamp() {
    (
        cd /app/share/daisy || exit 1
        find lib sample -type f -print0 \
            | sort -z \
            | xargs -0 sha256sum \
            | sha256sum \
            | cut -d' ' -f1
    )
}

bundled_data_stamp() {
    if [ -f "$BUNDLED_DATA_STAMP_FILE" ]; then
        cat "$BUNDLED_DATA_STAMP_FILE"
        return 0
    fi
    compute_bundled_data_stamp
}

sync_bundled_dir() {
    local source_dir="$1"
    local target_dir="$2"
    local temp_dir="${target_dir}.tmp.$$"

    rm -rf "$temp_dir"
    cp -a "$source_dir" "$temp_dir"
    rm -rf "$target_dir"
    mv "$temp_dir" "$target_dir"
}

bundled_data_stamp="$(bundled_data_stamp)"
installed_data_stamp=""
if [ -f "$DATA_STAMP_FILE" ]; then
    installed_data_stamp="$(cat "$DATA_STAMP_FILE")"
fi

# Refresh bundled runtime data when it is missing or when the packaged lib/sample
# content has changed, so reinstalls and upgrades do not leave stale copies under
# the persistent Flatpak data directory.
if [ ! -d "$DATA_DIR/lib" ] || [ ! -d "$DATA_DIR/sample" ] || [ "$bundled_data_stamp" != "$installed_data_stamp" ]; then
    sync_bundled_dir /app/share/daisy/lib "$DATA_DIR/lib"
    sync_bundled_dir /app/share/daisy/sample "$DATA_DIR/sample"
    printf '%s\n' "$bundled_data_stamp" > "$DATA_STAMP_FILE"
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
