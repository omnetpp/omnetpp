simple Animator {
    parameters:
        double animationSpeed = default(1);

        volatile double eventInterArrivalTime @unit(s); // in simulation time
        volatile double eventProcessingTime @unit(s) = default(0s); // in real time

        volatile double handleMessageHoldTime @unit(s) = default(0s);
        volatile double refreshDisplayHoldTime @unit(s) = default(0s);
}
