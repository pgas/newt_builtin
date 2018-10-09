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

    # newtSetSuspendCallback(suspend, NULL);
    # newtSetHelpCallback(helpCallback);

    newt DrawRootText 0 0 "Newt test program"
    newt PushHelpLine ""
    newt DrawRootText -50 0 "More root text"
    
    newt OpenWindow 2 2 30 10 "first window"
    newt OpenWindow 10 5 65 16 "window 2"
    
    
    newt -v f Form NULL "This is some help text" 0
    newt -v chklist Form NULL "" 0

    newt -v b1 Button 3 1 "Exit"
    newt -v b2 Button  18 1 "Update"
    newt -v r1 Radiobutton 20 10 "Choice 1" 0 NULL
    newt -v r2 Radiobutton 20 11 "Choice 2" 1 "$r1"
    newt -v r3 Radiobutton 20 12 "Choice 3" 0 "$r2"
    newt -v rsf Form NULL "" 0
    for c in r1 r2 r3; do
	newt FormAddComponent "$rsf" "${!c}"
    done

    for ((i = 0; i < 10; i+=1 ));do
	newt -v "cs[$i]" Checkbox 3 $((10+i)) "Check $i"
	echo >&2 "$? chk $i: ${cs[i]}"
	newt FormAddComponent "$chklist" "${cs[i]}"
    done
    
    newt -v l1 Label 3 6 "Scale:"
    newt -v l2 Label 3 7 "Scrolls:"
    newt -v l3 Label 3 8 "Hidden:"

    newt FormSetHeight "$chklist" 3
    
    for c in b1 b2 l1 l2 l3 chklist rsf;do
	newt FormAddComponent "$f" "${!c}"
    done

    newt RunForm "$f"

    newt FormDestroy "$f"
fi
