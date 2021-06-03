
network MultiSendTest
{
    @display("bgb=500,300");
    @figure[note](type=text;pos=50,50;text="Execute a few events.\nThen click on a line in the embedded log inspector below,\nin messages mode, showing a sending of a 'msg2' message.\nThis should correctly cause the embedded object inspector\nto switch to a 'msg2' message, and not a 'msg1' or 'msg3'.");
    submodules:
        prnt: Parent { @display("p=200,200"); };
        sink: Sink { @display("p=300,200"); };
    connections:
        prnt.out --> sink.in;
}
