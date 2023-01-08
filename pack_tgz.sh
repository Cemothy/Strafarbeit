for s in 1 2 3 4; do
    bt="aufgabe$s"
    eval cd $bt
    rm -f *.o $bt
    cd ..
    done

cd aufgabe5
rm -f *.o BinaryTree
cd ..


cd aufgabe6
rm -f *.o BinaryTreeAdvanced

cd ..

tar cfvz homework.tar.gz /mnt/c/ostep/strafarbeit