#!bin/bash




# hashing using djb2...bash version
# unsigned long
#     hash(unsigned char *str)
#     {
#         unsigned long hash = 5381;
#         int c;
#         while (c = *str++)
#             hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
#         return hash;
#     }

djb2() {
     hash=5381
     l=${1,,} # lowercase the words
     for ((i=0;i<${#l};i+=1)); do
 	 LC_CTYPE=C printf -v c %d "'${l:i:1}"
 	 (( hash = ((hash '<<' 5) + hash)+c ))
     done
     printf -v hs " %ul" $hash
     hash="${hs%l}ull"
}



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

declare -A subcommands
declare -A usage
declare -A fdecls

parse_fn () {
    paren_pos=0
    until [[ ${def[paren_pos]} =  "(" ]];do ((paren_pos+=1));done
    fname=${def[paren_pos-1]}
    fret=${def[*]:0:paren_pos-2+1}
#    echo >&2 "fname: $fname"
    break_fn_name "$fname"
    subco=${R%%_*}
    subco=${subco#[![:alpha:]]}     subco=${subco#[![:alpha:]]}
    fdecls[$subco]+=$'\n'"int $R(WORD_LIST * list);"
    echo >&2 "subco $subco"
    subcommands[$subco]=""
    usage[$subco]+=$'\n'"	newt $subco"
    fdef[$subco]+=$'\n'"int $R(WORD_LIST * list); {" 
    args=()
    cur_pos=paren_pos
    while [[ ${def[cur_pos]} !=  ")" ]];do
	end_arg=$((cur_pos+1))
	until [[ ${def[end_arg]} = [,\)] ]]; do ((end_arg+=1));done
	argtype=${def[@]:cur_pos+1:end_arg-cur_pos-2}
	argname=${def[end_arg-1]}
	args+="$argname, "
	usage[$subco]+=" $argname"
	#	echo  >&2 "argtype: $argtype, argname: $argname"
	
	case $argtype in
	    int|unsigned\ int)
		fdef[$subco]+=$'\n'"    READ_INT(list, $argname, ${subco^^}_USAGE);";;
	    *char*\*)
		fdef[$subco]+=$'\n'"    READ_STRING(list, $argname, ${subco^^}_USAGE);";;
	    newtComponent)
		fdef[$subco]+=$'\n'"    READ_COMPONENT(list, $argname, ${subco^^}_USAGE);";;
	    '')
		args=
		;;
	    *)
		fdef[$subco]+=$'\n'"    /* TODO: read $argname of $argtype */" ;;
	esac
	cur_pos=$end_arg
    done
    fdef[$subco]+=$'\n'
    case $fret in
	int)
	    fdef[$subco]+=$'\n'"    WRITE_INT(vname, $fname(${args%, });"
	    ;;
	*char*\*)
	    fdef[$subco]+=$'\n'"    WRITE_STRING(vname, $fname(${args%, });"
	    ;;
	newtComponent)
	    fdef[$subco]+=$'\n'"    WRITE_COMPONENT(vname, $fname(${args%, });"
	    ;;
	void)
	    fdef[$subco]+=$'\n'"    $fname(${args%, });"
	    ;;
	*)
	    fdef[$subco]+=$'\n'"/* TODO: returns $fret */"
	;;
    esac
    fdef[$subco]+=$'\n'"    return 0;"
    fdef[$subco]+=$'\n'"}"
    fdef[$subco]+=$'\n'
    fdef[$subco]+=$'\n'
    #echo >&2 "Fret: $fret"

}



# newtComponent newtCheckbox ( int left ,  int top ,  const char * text ,  char defValue , 			   const char * seq ,  char * result ) ;
while read -a def; do
    if [[ ${def[*]} = *\)* ]]; then
	parse_fn
    fi
done < <(extract_fn /usr/include/newt.h)

printf "%s\n" "${fdecls[@]}"

for so in "${!subcommands[@]}";do
    echo
    echo "static const char * ${so^^}_USAGE =    \\"
    echo '    "usage:"      \'
    echo "${usage[$so]}" | awk 'NF{printf "    \"%s\"  \\\n", $0}' | sort
    echo "${fdecls[$so]}"
    echo "${fdef[$so]}"
done

# <<EOF 
# void newtCheckboxSetFlags ( newtComponent co , int flags , enum newtFlagsSense sense ) ;
# newtComponent newtCompactButton ( int left ,  int top ,  const char * text ) ;
# newtComponent newtButton ( int left ,  int top ,  const char * text ) ;
# newtComponent newtCheckbox ( int left ,  int top ,  const char * text ,  char defValue , 			   const char * seq ,  char * result ) ;
# char newtCheckboxGetValue ( newtComponent co ) ;
# void newtCheckboxSetValue ( newtComponent co ,  char value ) ;
# void newtCheckboxSetFlags ( newtComponent co ,  int flags ,  enum newtFlagsSense sense ) ;

# EOF




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
