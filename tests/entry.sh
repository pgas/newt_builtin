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
    newt cls
    newt openwindow 10 7 40 7 "Button Sample"

    newt entry -v l1  10 1 "Hello, world!" 20 0

    newt button -v b1 10 3 "Ok"

    
    newt form -v form 
    newt form addcomponents "$form" "$l1" "$b1"
    
    newt runform "$form"

    newt form destroy "$form"
fi
