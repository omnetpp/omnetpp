//ERROR: wrong destination gate for connection: missing submodule index ('a' is a vector submodule)
simple Node
endsimple

module Network
    submodules:
        a: Node[5];
    connections:
        a.in <-- b.out;
endmodule
