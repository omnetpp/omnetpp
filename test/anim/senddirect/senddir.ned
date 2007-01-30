
simple Gen
{
}

simple Sink
{
    gates:
        input directIn;
        input in;
        input in2;
}

module BoxedSink
{
    gates:
        input directIn;
        input in;
    submodules:
        sink: Sink {
            parameters:
                @display("b=32,30;p=70,57;i=sink");
        };
    connections allowunconnected:
        directIn --> sink.in;
        in --> sink.in2;
}

module BoxedGen
{
    gates:
        output out1;
        output out2;
    submodules:
        gen: Gen {
            parameters:
                @display("b=32,30;p=68,69;i=gen");
        };
        boxedSink: BoxedSink {
            parameters:
                @display("p=164,97;b=40,28");
        };
        sink: Sink {
            parameters:
                @display("b=32,30;p=161,37;i=sink");
        };
}

module Dummy
{
    gates:
        output out;
    connections allowunconnected:
}


//
// Model for testing animation of sendDirect()
//
network SendDirTest
{
    submodules:
        boxedGen: BoxedGen {
            parameters:
                @display("p=71,109;b=40,28");
        };
        boxedSink: BoxedSink {
            parameters:
                @display("p=190,143;b=40,28");
        };
        sink: Sink {
            parameters:
                @display("b=32,30;p=192,84;i=sink");
        };
        dummy: Dummy {
            parameters:
                @display("p=127,40;i=ball_s;b=13,13");
        };
    connections allowunconnected:
        boxedGen.out1 --> sink.in;
        boxedGen.out2 --> boxedSink.in;
        dummy.out --> sink.in2;
}


