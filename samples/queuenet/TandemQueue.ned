//
// TODO Place comment here
//

network TandemQueue
{
    parameters:
        @display("i=block/network;bgb=468.0,224.0");
    submodules:
        queue2: Queue {
            @display("p=328.0,74.0");
        };
        queue1: Queue {
            @display("p=227.0,74.0");
        };
        queue: Queue {
            @display("p=127.0,74.0");
        };
        sink: Sink {
            @display("p=430.0,74.0");
        };
        source: Source {
            @display("p=32.0,74.0");
        };
    connections:
        source.out --> --> queue.in++;
        queue.out --> --> queue1.in++;
        queue1.out --> --> queue2.in++;
        queue2.out --> --> sink.in++;
}
