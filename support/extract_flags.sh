#!/bin/bash

# Extract flagss from newt.h
# bash > 4


if [[ $1 != *newt.h ]];then
   echo >&2  "usage: extract_newt_h /path/to/newt.h"
   exit 1
fi


awk '/^#define NEWT_COLORSET/{print $2, $3}' "$1" |
    while read -r name val;do
	echo "newt_bind_variable (\"$name\", \"$(($val))\", 0);"
    done




while read -r name val;do
    eval $name=$(($val))
    echo "newt_bind_variable (\"$name\", \"$(($val))\", 0);"
    done < <(awk '/^#define NEWT_FLAG/{print $2, $3$4$5}' "$1" )

awk '/^#define NEWT_(LIST|ENTR|TEXT|FORM)/{print $2, $3$4$5}' "$1" |
    while read -r name val;do
	echo "newt_bind_variable (\"$name\", \"${!val}\", 0);"
    done

grep '^enum newtFlagsSense' "$1" |
    while read -ra flags;do
	i=0
	for f in "${flags[@]}";do
	    [[ $f = NEWT_FLA* ]] || continue
	    f=NEWT_FLAGS_SENSE${f#NEWT_FLAGS}
	    echo "newt_bind_variable (\"${f%,}\", \"$((i++))\", 0);"
	done
    done

echo "newt_bind_variable (\"NEWT_CHECKBOXTREE_UNSELECTABLE\", \"$(((1 << 12)))\");"
echo "newt_bind_variable (\"NEWT_CHECKBOXTREE_HIDE_BOX\", \"$(((1 << 13)))\");"
echo "newt_bind_variable (\"NEWT_CHECKBOXTREE_COLLAPSED\", "");"
echo "newt_bind_variable (\"NEWT_CHECKBOXTREE_EXPANDED\", \"\\x01\");"
echo "newt_bind_variable (\"NEWT_CHECKBOXTREE_UNSELECTED\", \"\\x20\");"
echo "newt_bind_variable (\"NEWT_CHECKBOXTREE_SELECTED\", \"\\x2A\");"

