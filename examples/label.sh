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

if newt Init;then 
    trap 'newt Finished' EXIT
    newt Cls
    newt OpenWindow 10 7 40 7 "Button Sample"

    newt -v l1 Label   10 1 "Hello, world!"    
    newt -v b1 Button  10 3 "Ok"

    
    newt -v form Form "" "" 0
    newt FormAddComponent "$form" "$l1"
    newt FormAddComponent "$form" "$b1"
    
    newt RunForm "$form"
    newt LabelSetText "$l1" "Bye, world!"
    newt Refresh
    sleep 1
    newt LabelSetText "$l1" "The end, world!"
    newt Refresh
    sleep 1

    newt FormDestroy "$form"
fi
