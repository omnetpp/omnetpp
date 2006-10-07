//ERROR: wrong destination gate for connection: compound module has no submodule named 'a'
module Network
    connections:
        a.in <-- b.out;
endmodule
