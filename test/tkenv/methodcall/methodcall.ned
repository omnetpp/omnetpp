
simple Gen 
endsimple 

simple Sink 
endsimple 

module BoxedGen 
    submodules: 
        gen: Gen; 
            display: "b=32,30;p=68,69;i=gen";
        boxedSink: BoxedSink; 
            display: "p=164,97;b=40,28";
        sink: Sink; 
            display: "b=32,30;p=161,37;i=sink";
endmodule 

module BoxedSink 
    submodules: 
        sink: Sink; 
            display: "b=32,30;p=70,57;i=sink";
endmodule 

//
// Model for testing animation of method calls
//
module MethodAnimTest 
    submodules: 
        boxedGen: BoxedGen; 
            display: "p=71,109;b=40,28";
        boxedSink: BoxedSink; 
            display: "p=190,143;b=40,28";
        sink: Sink; 
            display: "b=32,30;p=192,84;i=sink";
endmodule 

network methodAnimTest : MethodAnimTest 
endnetwork 

