#!/bin/bash

#Make Universal Binary
gcc qssh.c -arch x86_64 -arch arm64 `find . \( -iname '*.c' -not -name 'qssh.c' \)` -o qssh
