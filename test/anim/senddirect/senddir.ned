
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
                @display("p=70,57;i=block/sink");
        }
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
                @display("p=68,69;i=block/source");
        }
        boxedSink: BoxedSink {
            parameters:
                @display("p=164,97;b=60,60");
        }
        sink: Sink {
            parameters:
                @display("p=161,37;i=block/sink");
        }
    connections allowunconnected:
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
                @display("p=71,109;b=60,60");
        }
        boxedSink: BoxedSink {
            parameters:
                @display("p=190,143;b=60,60");
        }
        sink: Sink {
            parameters:
                @display("p=192,84;i=block/sink");
        }
        dummy: Dummy {
            parameters:
                @display("p=127,40;i=misc/node_s");
        }
    connections allowunconnected:
        boxedGen.out1 --> sink.in;
        boxedGen.out2 --> boxedSink.in;
        dummy.out --> sink.in2;
}


