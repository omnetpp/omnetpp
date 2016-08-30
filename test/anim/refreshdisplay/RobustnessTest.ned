
simple RobustnessTester
{
    parameters:
        int raiseErrorAtEventNumber = default(-1);
        int raiseErrorAtRefreshNumber = default(-1);
        int raiseFigureRefreshErrorOnEventNumber = default(-1);
        bool raiseErrorInInitialize = default(false);
        bool raiseErrorInFinish = default(false);
}

network RobustnessTest
{
    parameters:
        @figure[dummy](type=oval;bounds=20,50,10,10);
        @figure[simtime](type=simtime;pos=20,100;color=green);
    submodules:
        tester: RobustnessTester { @display("p=100,200"); }
}
