#!/bin/bash
# port of the C Basic newt Example

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
    newt cls

    newt draw roottext 0 0 "some root text"
    newt draw roottext -25 -2 "Root text in the other corner"

    newt push helpline 
    newt refresh
    sleep 1

    newt push helpline "A help line"
    newt refresh
    sleep 1

    newt pop helpline
    newt refresh
    sleep 1

fi
