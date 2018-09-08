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
    newt openwindow 10 7 40 10 "Button Sample"


    function pos_filter {
	newt entry -v pos getcursorposition "$ent1"
	 newt entry set "$ent2" "filter $NEWT_CURSOR get $pos"
	 return 0
     }
    
    newt entry -v ent1  10 1 "move here" 20
    newt entry setfilter "$ent1" pos_filter

    newt entry -v pos getcursorposition "$ent1"
    echo >&2 "$ent1 $POS"
    newt entry -v ent2  10 2 "" 20 
    newt entry set "$ent2" "Cursor Position"
    
    newt entry -v ent3  10 4 "Hello, world!" 20 $((NEWT_FLAG_RETURNEXIT|NEWT_ENTRY_SCROLL))
    newt entry setfilter "$ent3" 'printf "ent:%s ch:%q cu:%s\n" $NEWT_ENTRY "$NEWT_CH" $NEWT_CURSOR >&2;[[ $NEWT_CH != e ]]'

    newt button -v b1 10 6 "Ok"

    
    newt form -v form 
    newt form addcomponents "$form" "$ent1" "$ent2" "$ent3" "$b1"
    
    newt runform "$form"
    newt entry -v message getvalue "$ent1"

    newt form destroy "$form"
    newt finished
fi
echo $message
