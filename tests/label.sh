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

    newt label -v l1  10 1 "Hello, world!"    
    newt button -v b1 10 3 "Ok"

    
    newt form -v form
    newt form addcomponents "$form" "$l1" "$b1"
    
    newt runform "$form"
        newt label settext "$l1" "Bye, world!"
    newt refresh
    sleep 1
    newt label settext "$l1" "The end, world!"
    newt refresh
    sleep 1

    newt form destroy "$form"
fi
