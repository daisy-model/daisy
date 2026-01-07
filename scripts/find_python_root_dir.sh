#!/bin/bash
if [[ "$#" -ne 1 ]]; then
    version=3.13
else
    version="$1"
fi
echo $( ls -d $( uv python dir | tr \\\\ / )/* | grep $version | sort -V | tail -n1 )
