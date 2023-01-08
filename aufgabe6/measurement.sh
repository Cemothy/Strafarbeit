#!/bin/sh

echo off

make clean
make

rm -f out.csv

cf="./BinaryTree -F 2"
eval $cf

for s in 2 4 6 8 10; do
    bt="./BinaryTreeAdvanced -n 4000 -p $s -F 1"
    eval $bt
    done

for s in 100 1000 2000 3000 4000; do
    bt="./BinaryTreeAdvanced -n $s -p 4 -F 1"
    eval $bt
    done