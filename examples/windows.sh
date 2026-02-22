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
# set -x
# newt centeredwindow
# newt centeredwindow 1
# set +x

if newt Init;then 
    trap 'newt Finished' EXIT
    newt Cls
    newt CenteredWindow 30 20 
    newt WaitForKey # also redraw
    newt CenteredWindow 25 10 "hello, world!"
    newt WaitForKey
    newt OpenWindow 1 1 10 5 
    newt WaitForKey
    newt OpenWindow 20 20 20 10 "hello, world!" 
    newt WaitForKey
    newt PopWindow
    newt WaitForKey
    newt PopWindow
    newt WaitForKey
    newt PopWindow
    newt WaitForKey
    newt PopWindow
    newt WaitForKey
fi
