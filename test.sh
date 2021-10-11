#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

tmp_dir=$(mktemp -d)
tested_program=$1
test_dir=$2

echo "Running correctness tests..."

for file in "$test_dir"/*.in
do
    filename=${file/$test_dir\//}
    f_stdout="$tmp_dir/${filename%in}out"
    f_stderr="$tmp_dir/${filename%in}err"

    "./$tested_program" < "$file" 1> "$f_stdout" 2> "$f_stderr"

#    if [ $? == 1 ]
#    then
#        echo "error code 1 -> out of memory"
#    else
        if diff -qZ "${file%in}out" "$f_stdout"
        then
            echo -e "${filename%in}out ${GREEN}OK${NC}"
        else
            echo -e "${filename%in}out ${RED}WRONG ANSWER${NC}"
        fi

        if diff -qZ "${file%in}err" "$f_stderr"
        then
            echo -e "${filename%in}err ${GREEN}OK${NC}"
        else
            echo -e "${filename%in}err ${RED}WRONG ANSWER${NC}"
        fi
#    fi
done

#valgrind_flags="--error-exitcode=123 --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all"
#
#echo "Running valgrind memory leaks tests..."
#
#for file in "$test_dir"/*.in
#do
#    filename=${file/$test_dir\//}
#
#    valgrind $valgrind_flags ./"$tested_program" < "$file" 1> /dev/null 2> "$tmp_dir/val.log"
#
#    if [ "$(cat "$tmp_dir/val.log" | grep -o "no leaks are possible")" == "no leaks are possible" ]
#    then
#        echo -e "$filename ${GREEN}valgrind memory test OK${NC}"
#    else
#        echo -e "$filename ${RED}valgrind memory test FAILED${NC}"
#    fi
#done
