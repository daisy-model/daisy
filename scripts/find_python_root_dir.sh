#!/bin/sh
uv_python_root=`uv python dir`
python_dir="$uv_python_root"/`ls "$uv_python_root" | grep 3.13`
echo "$python_dir"
