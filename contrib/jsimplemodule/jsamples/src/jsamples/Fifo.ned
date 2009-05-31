package jsamples;

import JSimpleModule;

simple Fifo extends JSimpleModule
{
    parameters:
        @display("i=block/queue;q=queue");
    gates:
        input in;
        output out;
}
