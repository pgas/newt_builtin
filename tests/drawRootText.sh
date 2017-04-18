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
set -x       
newt draw
newt draw roottext
newt draw roottext 2
newt draw roottext 4

set +x

if newt init;then 
    trap 'newt finished' EXIT
    newt cls
    newt draw roottext 10 10 "some root text"
    newt refresh
    sleep 2
fi


