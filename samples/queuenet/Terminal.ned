//
// TODO Place comment here
//

network Terminal
{
    parameters:
        @display("bgb=595.0,353.0;i=block/users");
    submodules:
        clerk: Server {
            @display("p=448.0,132.0");
        };
        toOtherTerminal: Sink {
            @display("p=124.0,225.0");
        };
        businessClass: PQueue {
            @display("p=329.0,132.0");
        };
        firstClass: PQueue {
            @display("p=329.0,39.0");
        };
        economyClass: PQueue {
            @display("p=329.0,225.0");
        };
        delay2: Delay {
            @display("p=222.0,132.0");
        };
        delay1: Delay {
            @display("p=222.0,39.0");
        };
        delay3: Delay {
            @display("p=222.0,225.0");
        };
        classifier: Classifier {
            @display("p=124.0,132.0");
        };
        boarding: Sink {
            @display("p=546.0,132.0;i=block/departure");
        };
        arrival: Source {
            @display("p=32.0,132.0;i=block/arrival");
        };
    connections:
        arrival.out --> classifier.in++;
        classifier.out++ --> delay1.in++;
        classifier.out++ --> delay2.in++;
        classifier.out++ --> delay3.in++;
        firstClass.out++ --> clerk.in++;
        businessClass.out++ --> clerk.in++;
        economyClass.out++ --> clerk.in++;
        clerk.out --> boarding.in++;
        delay1.out --> firstClass.in++;
        delay2.out --> businessClass.in++;
        delay3.out --> economyClass.in++;
        classifier.rest --> toOtherTerminal.in++;
}
