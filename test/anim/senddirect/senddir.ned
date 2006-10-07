
simple Gen 
endsimple 

simple Sink 
    gates: 
        in: directIn; 
        in: in; 
        in: in2; 
endsimple 

module BoxedSink 
    gates: 
        in: directIn; 
        in: in; 
    submodules: 
        sink: Sink; 
            display: "b=32,30;p=70,57;i=sink";
    connections nocheck: 
        directIn --> sink.in; 
        in --> sink.in2; 
endmodule 

module BoxedGen 
    gates: 
        out: out1; 
        out: out2; 
    submodules: 
        gen: Gen; 
            display: "b=32,30;p=68,69;i=gen";
        boxedSink: BoxedSink; 
            display: "p=164,97;b=40,28";
        sink: Sink; 
            display: "b=32,30;p=161,37;i=sink";
endmodule 

module Dummy 
    gates: 
        out: out; 
    connections nocheck: 
endmodule 


//
// Model for testing animation of sendDirect()
//
module SendDirTest 
    submodules: 
        boxedGen: BoxedGen; 
            display: "p=71,109;b=40,28";
        boxedSink: BoxedSink; 
            display: "p=190,143;b=40,28";
        sink: Sink; 
            display: "b=32,30;p=192,84;i=sink";
        dummy: Dummy; 
            display: "p=127,40;i=ball_s;b=13,13";
    connections nocheck: 
        boxedGen.out1 --> sink.in; 
        boxedGen.out2 --> boxedSink.in; 
        dummy.out --> sink.in2; 
endmodule 

network sendDirTest : SendDirTest 
endnetwork 

