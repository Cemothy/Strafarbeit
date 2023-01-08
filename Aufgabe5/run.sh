#!/bin/sh


make clean
make

for s in 1 2 4 8 16; do
    bt="./BinaryTree -n 1000 -p $s"
    eval $bt
done
