#!/bin/bash

if [[ -z $1 ]];then
    echo >&2 usage: gen_header.sh /path/to/newt.h
    exit 1
fi
gcc -nostdinc -E  -I ./utils/fake_libc_include/ -E "$1" > pre_processed_newt.h
python -m bash_builtin_gen bash_newt_h.tpl pre_processed_newt.h
