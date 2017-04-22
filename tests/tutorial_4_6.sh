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
    newt cls
    newt openwindow 10 5 40 8 "Entry and Label Sample"
    
    newt label -v label 1 1 "Enter a string"

    newt entry -v entry 16 1 "sample" 20 $((NEWT_FLAG_SCROLL | NEWT_FLAG_RETURNEXIT))
    newt button -v button 17 3 "Ok"

    
    newt form -v form

    newt form addcomponents "$form" "$label" "$entry" "$button"


    newt runform "$form"
    newt entry -v message getvalue "$entry"
    newt form destroy "$form"
    newt finished
fi
echo $message
