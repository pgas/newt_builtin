#!/bin/bash

# try possible locations
for lib in libs/ ../libs/;do
    if [[ -f $lib/newt.so ]];then
	enable -f "$lib"/newt.so newt
	break
    fi
done

if ! type newt &>/dev/null;then
    echo >&2 unable to load the new builtin
fi

if newt init;then 
    trap 'newt finished' EXIT

    #sleep to let time to fill the buffer
    sleep 2

    newt clearKeyBuffer
    newt bell
    newt waitForKey
fi
