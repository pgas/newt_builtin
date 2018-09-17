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

if newt Init;then
    trap 'newt Finished' EXIT
    newt Cls
    newt DrawRootText 10 10 "some root text"
    newt Refresh
    sleep 2
fi


