//
// TODO Place comment here
//

network RingQueue
{
    parameters:
        @display("i=device/lan-ring;bgb=434.0,353.0");
    submodules:
        source1: Source {
            @display("p=370.0,36.0");
        };
        source: Source {
            @display("p=54.0,45.0");
        };
        queue: Queue {
            @display("p=110.0,124.0");
        };
        queue1: Queue {
            @display("p=139.0,241.0");
        };
        queue2: Queue {
            @display("p=282.0,251.0");
        };
        queue3: Queue {
            @display("p=327.0,134.0");
        };
        queue4: Queue {
            @display("p=232.0,64.0");
        };
    connections:
        queue4.out --> --> queue.in++;
        queue.out --> --> queue1.in++;
        queue1.out --> --> queue2.in++;
        queue2.out --> --> queue3.in++;
        queue3.out --> --> queue4.in++;
        source.out --> --> queue.in++;
        source1.out --> --> queue3.in++;
}
