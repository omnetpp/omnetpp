//ERROR: submodule 'a' is a vector submodule
simple Node
endsimple

module Network
    submodules:
        a: Node[5];
    connections:
        a.out --> b.in;
endmodule
