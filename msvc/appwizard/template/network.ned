//-------------------------------------------------------------
// file: $$root$$.ned
//-------------------------------------------------------------


$$BEGINLOOP(SIMPLE_N)
import "$$MODULE_NAME$$.ned";
$$ENDLOOP // SIMPLE_N


// $$root$$ --
//
// $$PROJECT_DESCRIPTION$$
//
module $$root$$
    parameters:
        // ...
    submodules: 
        // ...
    connections:
        // ...
endmodule 


// the$$root$$ --
//
// Instantiates the $$root$$ network
//
network the$$root$$ : $$root$$
    parameters: 
        // ... (parameter assignments)
endnetwork 
