#!/bin/bash
# port of the C Button Example

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
    
    newt -v b1 Button 10 1 "Ok"
    newt -v b2 CompactButton  22 2 "Cancel"
    newt ComponentAddDestroyCallback "$b1" 'printf Hello, >&2'
    newt ComponentAddDestroyCallback "$b2" 'echo \ World! >&2'
    
    newt -v form Form NULL "a help string?" 0
    newt FormAddComponent "$form" "$b1"
    newt FormAddComponent "$form" "$b2"

    newt FormDestroy "$form"
fi
