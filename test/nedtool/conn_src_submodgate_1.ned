//ERROR: wrong source gate for connection: submodule 'a' has no gate named 'out'
simple Node
endsimple

module Network
    submodules:
        a: Node;
    connections:
        a.out --> b.in;
endmodule
