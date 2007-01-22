//
// TODO Place comment here
//

network SimpleQueue
{
    parameters:
        @display("bgb=335.0,194.0;i=block/network2");
    submodules:
        sink: Sink {
            @display("p=276.0,79.0");
        };
        queue: Queue {
            @display("p=165.0,79.0");
        };
        source: Source {
            @display("p=50.0,79.0");
        };
    connections:
        source.out --> --> queue.in++;
        queue.out --> --> sink.in++;
}
