#!/bin/sh

echo off

make clean
make

rm -f out.csv

cf="./aufgabe3 -F 2"
eval $cf

for s in 2 4 6 8 10; do
    bt="./aufgabe3 -t $s -T 4 -F 1"
    eval $bt
    done

for s in 1 2 4 8 16 32; do
    bt="./aufgabe3  -t 4 -T $s -F 1"
    eval $bt
    done