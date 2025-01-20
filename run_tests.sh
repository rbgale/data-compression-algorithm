#!/bin/bash
# Script to run all test cases


strip_path() { printf '%s' ${1##*/}; }

usage() { printf 'Usage: %s [-m <9..20>] [-p] [-h] [-v]\n\n' "$1"; }

help_display() {
    printf 'Description: Script to run the test cases in the /test_cases folder.\n'
    usage $(strip_path $0)
    printf '%s\n' \
        'Options:' \
        '-m    Specify the maximum bit size to be tested (default is 12).' \
        '-p    Test data without pruning (enabled by default).' \
        '-h    Print this usage information.' \
        '-v    Verbose report of passed/failed files.'
}

grade() {
    printf '%s ' $1

    # Add spacing to right-align text
    let "n=((65 - ${#1}))"
    for ((i=1;i<=n;i++)); do
        printf '-';
    done

    if $2 = true; then
        printf ' %s\n' 'PASS'
    else
        printf ' %s\n' 'FAIL'
    fi
}

# Default values for max bits and pruning
max_bits=12
prune=true
verbose=false

# Get arguments
while getopts "mphv" opt; do
    case "$opt" in
        m )
            max_bits="$OPTARG"
            ;;
        p )
            prune="$OPTARG"
            ;;
        h )
            help_display
            exit 0
            ;;
        v )
            verbose=1
            ;;
        ? )
            usage
            echo "Run \"$(strip_path $0) -h\" for more options."
            ;;
    esac
done

# Update files
make -s clean
make -s

# Encode/decode test files and compare results with original files
num_passed=0
total=0

for file in ./test_cases/*; do
    total=$((total + 1))

    file_name=$(strip_path $file)

    # Toggle pruning
    p=""
    if [ "$prune" = true ]; then
        p="-p"
    fi

    # Grade current file
    if ./encode -m $max_bits $p < $file | ./decode | cmp - $file; then
        grade "$file_name" true

        num_passed=$((num_passed + 1))
    else
        grade "$file_name" false
    fi

    # Display location and detail of errors if verbose is set
    if [ "$verbose" = true ]; then
        diff <(cat $file | ./encode -m max_bits $p | ./decode) <(cat $file);
    fi    
done

printf '\nResult: %d/%d test cases passed.' $num_passed $total

make -s clean

