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
set -x       

newt push
newt push blah
newt push helpline
newt push helpline "foobar"

newt pop
newt pop helpline
