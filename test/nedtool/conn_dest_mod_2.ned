//ERROR: wrong destination gate for connection: submodule 'a' is a vector submodule
simple Node
endsimple

module Network
    submodules:
        a: Node[5];
    connections:
        a.in <-- b.out;
endmodule
