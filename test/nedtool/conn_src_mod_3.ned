//ERROR: submodule 'a' is not a vector submodule
simple Node
endsimple

module Network
    submodules:
        a: Node;
    connections:
        a[0].out --> b.in;
endmodule
