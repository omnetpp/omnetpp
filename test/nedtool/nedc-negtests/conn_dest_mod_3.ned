//ERROR: wrong destination gate for connection: extra submodule index ('a' is not a vector submodule)
simple Node
endsimple

module Network
    submodules:
        a: Node;
    connections:
        a[0].in <-- b.out;
endmodule
