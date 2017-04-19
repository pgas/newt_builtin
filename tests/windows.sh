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
    trap 'newt finished' EXIT
    newt cls
    newt centeredwindow 60 20 
    newt waitForKey # also redraw
    newt centeredwindow 40 10 "hello, world!"
    newt waitForKey
    newt pop window
    newt waitForKey
    newt pop window
    newt waitForKey
fi
