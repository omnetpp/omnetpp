//ERROR: wrong destination gate for connection: submodule 'a' has no gate named 'in'
simple Node
endsimple

module Network
    submodules:
        a: Node;
    connections:
        a.in <-- b.out;
endmodule
