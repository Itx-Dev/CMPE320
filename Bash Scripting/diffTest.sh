#!/bin/bash

n=1  # Replace 5 with the value of your variable n

expectedFile="tests/$n.out"
outputFile="tests-out/$n.out"
filetype="out"

if ! diff -q "$expectedFile" "$outputFile" >/dev/null; then
    echo "  what results should be found in file: $expectedFile"
    echo "  what results produced by your program: $outputFile"
    echo "  compare the two using diff, cmp, or related tools to debug, e.g.:"
    echo "  prompt> diff $outputFile $expectedFile"
fi