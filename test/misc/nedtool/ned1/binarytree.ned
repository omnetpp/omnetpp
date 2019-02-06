//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


simple BinaryTreeNode
    gates:
        in: fromupper;
        out: downleft;
        out: downright;
endsimple


module BinaryTree1
    parameters:
        height: numeric const;
    submodules:
        node: BinaryTreeNode[ 2^height-1 ];
            display: "i=misc/node_vs";
    connections nocheck:
        for i=0..2^height-2, j=0..2^height-2 do
            node[i].downleft --> node[j].fromupper if j==2*i+1;
            node[i].downright --> node[j].fromupper if j==2*i+2;
        endfor;
endmodule


module BinaryTree2
    parameters:
        height: numeric const;
    submodules:
        node: BinaryTreeNode[ 2^height-1 ];
            display: "i=misc/node_vs";
    connections nocheck:
        for i=0..2^(height-1)-2 do
            node[i].downleft --> node[2*i+1].fromupper;
            node[i].downright --> node[2*i+2].fromupper;
        endfor;
endmodule


network binaryTree1 : BinaryTree1
    parameters:
        height = input(5,"Height of the tree");
endnetwork


network binaryTree2 : BinaryTree2
    parameters:
        height = input(5,"Height of the tree");
endnetwork

