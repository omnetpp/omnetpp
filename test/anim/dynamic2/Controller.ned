simple Controller
{
    parameters:
        @display("i=block/cogwheel");
        volatile int numToCreate = default(50);
        volatile double deleteProbability = default(0.5);
        volatile double waitTime @unit(s) = default(5s);
}
