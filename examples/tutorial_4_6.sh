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
    newt Cls
    newt OpenWindow 10 5 40 8 "Entry and Label Sample"
    
    newt -v label Label 1 1 "Enter a string"

    newt -v entry Entry 16 1 "sample" 20 $((NEWT_FLAG_SCROLL | NEWT_FLAG_RETURNEXIT))
    newt -v button Button 17 3 "Ok"

    
    newt -v form Form

    newt FormAddComponents "$form" "$label" "$entry" "$button"


    newt RunForm "$form"
    newt -v message EntryGetValue "$entry"
    newt FormDestroy "$form"
    newt Finished
fi
echo $message
