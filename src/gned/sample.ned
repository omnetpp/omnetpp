// ------------------------------------------------------------------------
// File: sample.ned
// 
// This is a file comment. File comments reach from the top of the file
// till the last blank line above the first code line.
// ------------------------------------------------------------------------ 

// 
// The file comment can also contain blank lines, so this is still part of
// the above file comment.
// 

// Module1 --
// 
// This is a banner comment for the Module1 declaration below.
// Banner comments can be multi-line, but they are not supposed to contain
// blank lines. (Otherwise the lines above the blank one will be taken as
// part of a file comment or trailing comment.)
// 
module Module1 
    submodules: // and this is right-comment
        // This is another banner comment, for the submodule
        submod1: Module; 
            display: "p=128,156;b=96,72,rect";
    connections: 
        out --> submod1.in; // Right-comments can also be 
                            // multi-line.
endmodule 

// Finally, this is a trailing comment, belonging to the above module.
// It may contain blank lines. Trailing comments are mostly used to put
// separator lines into the file, like this:
// -----------------------------------------------------------------------




simple Simple1 
endsimple 

// Module2 --
// 
// an empty module
// 
module Module2 
endmodule 










