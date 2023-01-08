#!/bin/sh

echo off

make clean
make

for s in 1 2 4 8 16; do
    bt="./BinaryTree -n 1000 -p $s"
    eval $bt
done

for s in 2 4 6 8 10; do
    bt="./BinaryTree -n 1000 -p $s"
    eval $bt