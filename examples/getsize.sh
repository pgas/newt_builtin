#!/bin/bash

# try possible locations
for lib in libs/ ../libs/ build/src/ ../build/src/;do
    if [[ -f $lib/newt.so ]];then
	enable -f "$lib"/newt.so newt
	break
    fi
done

if ! type newt &>/dev/null;then
    echo >&2 unable to load the new builtin
fi

if newt Init;then 
    trap 'newt Finished' EXIT
    newt GetScreenSize NEWT_COLS NEWT_ROWS
    newt Finished
    echo >&2 "${NEWT_COLS}x${NEWT_ROWS}"
fi
