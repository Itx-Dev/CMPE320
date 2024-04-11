#! /bin/bash

for file in *.c; do
	if [ -f "$file" ]; then
		grep "printf" "$file"
	fi
done