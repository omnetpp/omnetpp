import "classifier.ned";
import "delay.ned";
import "source.ned";
import "pqueue.ned";
import "qnet.ned";
import "server.ned";
import "sink.ned";

module QNet
    submodules:
        src: QSource;
            parameters:
            display: "p=63,130;i=gen";
        classifier: QClassifier;
            gatesizes:
                out[3];
            display: "p=160,130;i=fork";
        d: QDelay[3];
            display: "p=263,70,column;i=delay";
        passiveq: QPassiveQueue[3];
            gatesizes:
                out[1];
            display: "p=349,70,column;i=proc2;q=queue";
        qserver: QServer;
            gatesizes:
                in[3];
            display: "p=443,130;i=queue";
        sink: QSink;
            display: "p=546,130;i=sink";
        drop: QSink;
            display: "p=160,200;i=sink";
    connections:
        src.out --> classifier.in;
        for i=0..2 do
            classifier.out[i] --> d[i].in;
            d[i].out --> passiveq[i].in;
            passiveq[i].out[0] --> qserver.in[i];
        endfor;
        qserver.out --> sink.in;
        classifier.other --> drop.in;
endmodule

network qnet : QNet
endnetwork
