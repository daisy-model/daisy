#!/bin/bash
if [[ -v 1 ]]; then
    version="$1"
else
    version=3.13
fi
echo $( ls -d $( uv python dir | tr \\\\ / )/* | grep $version | sort -V | tail -n1 )
