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

if newt init;then 

    newt cls
    newt openwindow 10 7 40 7 "Button Sample"

    newt entry -v ent1  10 1 "Hello, world!" 20 $((NEWT_FLAG_RETURNEXIT|NEWT_ENTRY_SCROLL))
    newt entry setfilter "$ent1" 'printf "ent:%s ch:%q cu:%s\n" $NEWT_ENTRY "$NEWT_CH" $NEWT_CURSOR >&2;[[ $NEWT_CH != e ]]'
    newt button -v b1 10 3 "Ok"

    
    newt form -v form 
    newt form addcomponents "$form" "$ent1" "$b1"
    
    newt runform "$form"
    newt entry -v message getvalue "$ent1"

    newt form destroy "$form"
    newt finished
fi
echo $message
