#!/bin/bash
# port of the C Basic newt Example

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

    newt DrawRootText 0 0 "some root text"
    help="Root text in the other corner"
    newt DrawRootText -${#help} -2 "$help"

    newt PushHelpLine ""
    newt Refresh
    sleep 1

    newt PushHelpLine "A help line"
    newt Refresh
    sleep 1

    newt PopHelpLine
    newt Refresh
    sleep 1

fi
