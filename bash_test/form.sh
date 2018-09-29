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



if newt Init;then 
    newt Cls
    newt CenteredWindow 60 20 "Form Test"
    newt -v myform Form NULL "" 0
    newt WaitForKey # also redraw
    newt FormDestroy $myform
    newt WaitForKey # also redraw
    newt Finished
fi
echo  "myform: $myform"
