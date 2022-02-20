
simple Node
{
    int bar;

    @signal[foo1];
    @signal[foo2];

    @statistic[foo1](source=foo1; record=mean,last,stats,histogram,vector; interpolationmode=sample-hold);
    @statistic[foo2](source=foo2; record=mean,last,stats,histogram,vector; title="A very nice foo.");
}

network Test
{
    submodules:
        node1: Node;
        node2: Node;
}

