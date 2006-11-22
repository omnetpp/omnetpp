//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


simple BinaryTreeNode
{
    gates:
        input fromupper;
        output downleft;
        output downright;
}


module BinaryTree1
{
    parameters:
        int height;
    submodules:
        node[2^height-1]: BinaryTreeNode {
            parameters:
                @display("i=misc/node_vs");
        };
    connections allowunconnected:
        for i=0..2^height-2, for j=0..2^height-2 {
            node[i].downleft --> node[j].fromupper if j==2*i+1;
            node[i].downright --> node[j].fromupper if j==2*i+2;
        };
}


module BinaryTree2
{
    parameters:
        int height;
    submodules:
        node[2^height-1]: BinaryTreeNode {
            parameters:
                @display("i=misc/node_vs");
        };
    connections allowunconnected:
        for i=0..2^(height-1)-2 {
            node[i].downleft --> node[2*i+1].fromupper;
            node[i].downright --> node[2*i+2].fromupper;
        };
}


network binaryTree1 extends BinaryTree1
{
    parameters:
        height = default(5) @prompt("Height of the tree");
}


network binaryTree2 extends BinaryTree2
{
    parameters:
        height = default(5) @prompt("Height of the tree");
}

