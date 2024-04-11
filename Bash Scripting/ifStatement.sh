#!/bin/bash

prefile="test$n.pre"

if [[ -f "$prefile" && -x "$prefile" ]]; then
    ./"$prefile"
else
    echo "$prefile does not exist or is not executable."
fi