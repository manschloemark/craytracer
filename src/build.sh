#!/usr/bin/env bash
filename=$(git branch --show-current).exe
echo "Compiling $filename"

#echo "Starting $filename compilation..."
#for cfile in *.c; do
#	echo "    Compiling $cfile..."
#	gcc -c "$cfile" -o ../build/obj/"${cfile%.c}.o"
#done
gcc $@ ./*.c -o ../build/$filename -lm
