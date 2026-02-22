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
    newt OpenWindow 10 7 40 10 "Button Sample"


    function pos_filter {
	newt -v pos EntryGetCursorPosition "$ent1"
	newt EntrySet "$ent2" "filter $NEWT_CURSOR get $pos" 0
	return 0
     }
    
    newt -v ent1 Entry 10 1 "move here" 20
    newt EntrySetFilter "$ent1" pos_filter

    newt -v pos EntryGetCursorPosition "$ent1"
    echo >&2 "$ent1 $pos"
    newt -v ent2 Entry 10 2 "" 20
    newt EntrySet "$ent2" "Cursor Position" 0
    
    newt -v ent3 Entry 10 4 "Hello, world!" 20 $((NEWT_FLAG_RETURNEXIT|NEWT_ENTRY_SCROLL))
    newt EntrySetFilter "$ent3" 'printf "ent:%s ch:%q cu:%s\n" $NEWT_ENTRY "$NEWT_CH" $NEWT_CURSOR >&2;[[ $NEWT_CH != e ]]'

    newt -v b1 Button 10 6 "Ok"

    
    newt -v form Form
    newt FormAddComponents "$form" "$ent1" "$ent2" "$ent3" "$b1"
    
    newt RunForm "$form"
    newt -v message EntryGetValue "$ent1"

    newt FormDestroy "$form"
    newt Finished
fi
echo $message
