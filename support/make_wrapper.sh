#!bin/bash


extract_fn () {
# another parsing attempt....
# first line strips cpp, typedefs and comments
# second line grabe the fundefs and put them on one line
awk '/^#/||/typedef/{next}/^\/\*/{f=1}!f{print}/\*\//{f=0}' "$1" \
    |  awk -v RS=')' -F'(' 'NF>1{sub(/.*\n/,"",$(NF-1));gsub(/\n/,"",$NF);gsub(/,/, " , ",$NF);print $(NF-1)" ( "$NF" ) ;"}'
}


break_fn_name () {
    local i s c
    s=${1#newt} s=${s,}
    R=
    for (( i=0;i<${#s};i+=1)); do
	c=${s:i:1}
	if [[ $c = [[:upper:]] ]]; then
	    c=_${c,}
	fi
	R+=$c
    done
}

parse_fn () {
    paren_pos=0
    until [[ ${def[paren_pos]} =  "(" ]];do ((paren_pos+=1));done
    fname=${def[paren_pos-1]}
    fret=${def[*]:0:paren_pos-2+1}
#    echo >&2 "fname: $fname"
    break_fn_name "$fname"
    fdecls+=("int $R(WORD_LIST * list);")

    fdef=()
    fdef+=("int $R(WORD_LIST * list); {" )
    args=()
    cur_pos=paren_pos
    while [[ ${def[cur_pos]} !=  ")" ]];do
	end_arg=$((cur_pos+1))
	until [[ ${def[end_arg]} = [,\)] ]]; do ((end_arg+=1));done
	argtype=${def[@]:cur_pos+1:end_arg-cur_pos-2}
	argname=${def[end_arg-1]}
	args+="$argname, "
	#	echo  >&2 "argtype: $argtype, argname: $argname"
	
	case $argtype in
	    int|unsigned\ int)
		fdef+=("    READ_INT(list, $argname, ENTRY_USAGE);");;
	    *char*\*)
		fdef+=("    READ_STRING(list, $argname, ENTRY_USAGE);");;
	    newtComponent)
		fdef+=("    READ_COMPONENT(list, $argname, ENTRY_USAGE);");;
	    '')
		args=
		;;
	    *)
		fdef+=("    /* TODO: read $argname of $argtype */") ;;
	esac
	cur_pos=$end_arg
    done
    printf "%s\n" "${fdef[@]}" ""
    case $fret in
	int)
	    echo "    WRITE_INT(vname, $fname(${args%, });"
	    ;;
	*char*\*)
	    echo "    WRITE_STRING(vname, $fname(${args%, });"
	    ;;
	newtComponent)
	    echo "    WRITE_COMPONENT(vname, $fname(${args%, });"
	    ;;
	void)
	    echo "    $fname(${args%, });"
	    ;;
	*)
	    echo "/* TODO: returns $fret */"
	;;
    esac
    echo "    return 0;"
    echo "}"
    #echo >&2 "Fret: $fret"

}



# newtComponent newtCheckbox ( int left ,  int top ,  const char * text ,  char defValue , 			   const char * seq ,  char * result ) ;
while read -a def; do
    if [[ ${def[*]} = *\)* ]]; then
	parse_fn
    fi
done < <(extract_fn /usr/include/newt.h)

# <<EOF 
# void newtCheckboxSetFlags ( newtComponent co , int flags , enum newtFlagsSense sense ) ;
# newtComponent newtCompactButton ( int left ,  int top ,  const char * text ) ;
# newtComponent newtButton ( int left ,  int top ,  const char * text ) ;
# newtComponent newtCheckbox ( int left ,  int top ,  const char * text ,  char defValue , 			   const char * seq ,  char * result ) ;
# char newtCheckboxGetValue ( newtComponent co ) ;
# void newtCheckboxSetValue ( newtComponent co ,  char value ) ;
# void newtCheckboxSetFlags ( newtComponent co ,  int flags ,  enum newtFlagsSense sense ) ;

# EOF

printf "%s\n" "${fdecls[@]}"

exit

left=${1%%\(*}
fname=newt${left##*newt}
echo $fname
f=${fname#newt}
f=${f,}
sub="${f%%[[:upper:]]*}"

command=${f#${f%%[[:upper:]]*}}
command=${command,,}
USAGE="newt $sub $command"

params="${1#"$left("}"		  
IFS=',' read -ra args <<< "${params%%)*}"
for a in "${args[@]}";do
    read -a words <<< "$a"
    argname=${words[-1]}
    USAGE+=" $argname"
    case $a in
	newtComponent*)
	    echo "NEXT(list, ${sub^^}_${command^^}_USAGE);"
	    echo "bash_component $argname";
	    echo "READ_COMPONENT(list->word->word, $argname, \"Invalid component\");"
	    ;;
	*)
	    echo a:$a
	   ;;
    esac	    
done
echo $USAGE
