
simple Mod 
endsimple 

simple Target 
endsimple 

module BoxedTarget 
    submodules: 
        target: Target; 
            display: "b=32,30;p=70,57;i=sink";
endmodule 

module BoxedMod 
    submodules: 
        mod: Mod; 
            display: "b=32,30;p=68,69;i=gen";
        boxedTarget: BoxedTarget; 
            display: "p=220,97;b=40,28";
        target: Target; 
            display: "b=32,30;p=220,37;i=sink";
endmodule 

//
// Model for testing animation of method calls
//
module MethodAnimTest 
    submodules: 
        boxedMod: BoxedMod; 
            display: "p=71,109;b=40,28";
        boxedTarget: BoxedTarget; 
            display: "p=230,143;b=40,28";
        target: Target; 
            display: "b=32,30;p=230,84;i=sink";
endmodule 

network methodAnimTest : MethodAnimTest 
endnetwork 

