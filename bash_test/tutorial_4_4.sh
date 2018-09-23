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

if newt Init;then 
    trap 'newt Finished' EXIT
    newt Cls
    newt OpenWindow 10 5 40 6 "Button Sample"
    
    newt -v b1 Button 10 1 "Ok"
    newt -v b2 CompactButton  22 2 "Cancel"
    
    newt -v form Form NULL "a help string?" 0
    

    newt FormAddComponent "$form" "$b1"
    newt FormAddComponent "$form" "$b2"
    newt Refresh

    newt RunForm "$form"

    newt FormDestroy "$form"
fi
