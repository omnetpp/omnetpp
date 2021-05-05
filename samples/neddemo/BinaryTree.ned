//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


simple BinaryTreeNode extends Node
{
    parameters:
        @display("i=misc/node2_vs");
    gates:
        inout toParent;
        inout leftChild;
        inout rightChild;
}


network BinaryTree1
{
    parameters:
        int height @prompt("Height of the tree") = default(5);
    submodules:
        node[2^height-1]: BinaryTreeNode;
    connections allowunconnected:
        for i=0..2^height-2, for j=0..2^height-2 {
            node[i].leftChild <--> node[j].toParent if j==2*i+1;
            node[i].rightChild <--> node[j].toParent if j==2*i+2;
        }
}


network BinaryTree2
{
    parameters:
        int height @prompt("Height of the tree") = default(5);
    submodules:
        node[2^height-1]: BinaryTreeNode;
    connections allowunconnected:
        for i=0..2^(height-1)-2 {
            node[i].leftChild <--> node[2*i+1].toParent;
            node[i].rightChild <--> node[2*i+2].toParent;
        }
}


