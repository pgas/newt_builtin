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
# set -x
# newt centeredwindow
# newt centeredwindow 1
# set +x

if newt init;then 
#    trap 'newt finished' EXIT
    newt cls
    newt centeredwindow 60 20
    newt form -v myform
    newt waitForKey # also redraw
    newt finished
fi
echo  "myform: $myform"
