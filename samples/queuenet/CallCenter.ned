//
// TODO Place comment here
//

network CallCenter
{
    parameters:
        @display("bgb=626.0,422.0;bgl=2");
    submodules:
        sink1: Sink {
            @display("p=132.0,362.0");
        };
        delay: Delay {
            @display("p=283.0,355.0");
        };
        router: Router {
            @display("p=491.0,268.0");
            gates:
                out[2];
        };
        sink: Sink {
            @display("p=550.0,165.0");
        };
        server0: Server {
            @display("p=367.0,58.0");
        };
        pqueue0: PQueue {
            @display("p=226.0,59.0");
        };
        classifier: Classifier {
            @display("p=131.0,268.0");
        };
        source: Source {
            @display("p=35.0,268.0");
        };
        pqueue1: PQueue {
            @display("p=226.0,136.0");
        };
        pqueue2: PQueue {
            @display("p=226.0,201.0");
        };
        pqueue3: PQueue {
            @display("p=226.0,268.0");
        };
        server012: Server {
            @display("p=367.0,136.0");
        };
        server2: Server {
            @display("p=367.0,201.0");
        };
        server3: Server {
            @display("p=367.0,268.0");
        };
    connections:
        source.out --> --> classifier.in++;
        classifier.out++ --> --> pqueue0.in++;
        classifier.out++ --> --> pqueue1.in++;
        classifier.out++ --> --> pqueue2.in++;
        classifier.out++ --> --> pqueue3.in++;
        pqueue0.out++ --> --> server0.in++;
        pqueue0.out++ --> --> server012.in++;
        pqueue1.out++ --> --> server012.in++;
        pqueue2.out++ --> --> server012.in++;
        pqueue2.out++ --> --> server2.in++;
        pqueue3.out++ --> --> server3.in++;
        server0.out --> --> router.in++;
        server012.out --> --> router.in++;
        server2.out --> --> router.in++;
        server3.out --> --> router.in++;
        router.out[0] --> --> sink.in++;
        router.out[1] --> --> delay.in++;
        delay.out --> --> classifier.in++;
        classifier.rest --> --> sink1.in++;
}
