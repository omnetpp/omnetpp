simple Animator {
    parameters:
        double animationSpeed = default(1);

        volatile double eventInterArrivalTime @unit(s); // in simulation time
        volatile double eventProcessingTime @unit(s) = default(0s); // in real time

        double holdInProcessMessageProb = default(0);
        volatile double holdInProcessMessageTime @unit(s) = default(1s);

        double holdInRefreshDisplayProb = default(0);
        volatile double holdInRefreshDisplayTime @unit(s) = default(1s);
}
