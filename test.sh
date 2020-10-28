#!/bin/bash

if [[ $# != 2 ]]; then
  echo "Sposób uzytkowania: $0 <ścieżka/do/folderu/z/testami> <ścieżka/do/folderu/z/projektem> " >&2
  exit 1
fi

tests=$(realpath "$1")
project=$(realpath "$2")
threshold=${3:-1}

if ! [[ -d "$tests" ]]; then
  echo "Podany folder z testami nie istnieje"
  exit 1
fi

if ! [[ -d "$project" ]]; then
  echo "Podany folder z projektem nie istnieje"
  exit 1
fi

total=0
correct=0
leaked=0

function traverse_folder() {
  folder="$1"
  shopt -s nullglob
  for f in "$folder"/*.in; do
    rand_float="$(printf '0%s\n' "$(echo "scale=8; $RANDOM/32768" | bc )")"
    if (( $(echo "$rand_float < $threshold" |bc -l) )); then
      run_test "$f"
    fi
  done

  shopt -s nullglob
  for d in "$folder"/*/; do
    echo "$d"
    traverse_folder "$(realpath "$d")"
  done
}

RED='\033[0;31m'
GREEN='\033[0;32m'
NOCOLOR='\033[0m'

function run_test() {
  input_file="$1"
  output_file=${input_file//.in/.out}
  error_file=${input_file//.in/.err}

  ((total++))
  echo -e "\e[1mTest $f \e[0m"

  ./nod < "$input_file" 1>"$temp_out" 2>"$temp_err"

    if cmp -s "$output_file" "$temp_out" ; then
        echo -ne "${GREEN}stdout ok${NOCOLOR}, "

        if cmp -s "$error_file" "$temp_err" ; then
            echo -ne "${GREEN}stderr ok${NOCOLOR}\n"
            ((correct++))
        else
            echo -ne "${RED}stderr nieprawidlowe${NOCOLOR}\n"
            diff -d "$error_file" "$temp_err"
        fi

    else
        echo -ne "${RED}stdout nieprawidlowe${NOCOLOR}\n"
        diff -d "$output_file" "$temp_out"
    fi
  }

temp_out=$(mktemp)
temp_err=$(mktemp)
trap 'rm -f "$temp_out $temp_err" nod' INT TERM HUP EXIT

cd "$2" || exit
g++ -Wall -Wextra -O2 -std=c++17 nod.cc -o nod

traverse_folder "$tests"

echo "total: ${total}, correct ${correct}"
