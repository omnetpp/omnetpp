simple RefreshDisplay
{
    parameters:
        int raiseErrorAtEventNumber = default(-1);
        int raiseErrorAtRefreshNumber = default(-1);
        bool raiseErrorInInitialize = default(false);
        bool raiseErrorInFinish = default(false);
}

network RefreshDisplayTest
{
    submodules:
        refreshDisplay: RefreshDisplay { @display("p=100,100"); }
}
