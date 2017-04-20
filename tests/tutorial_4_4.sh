#!/bin/bash
# port of the C Button Example

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
    newt openwindow 10 5 40 6 "Button Sample"

    newt button -v b1 10 1 "Ok"
    newt compactbutton -v b2 22 2 "Cancel"
    
    newt form -v form
    newt form addcomponents "$form" "$b1" "$b2"

    newt runform "$form"
    newt form destroy "$form"
fi
