#!/usr/bin/env bash
filename=$(git branch --show-current).exe
gcc $@ main.c -o ../build/$filename -lm

