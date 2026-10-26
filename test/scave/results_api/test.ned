
simple Node
{
    int bar;

    @signal[foo1];
    @signal[foo2];
    @signal[zero];

    @statistic[foo1](source=foo1; record=mean,last,stats,histogram,vector; unit=ms; interpolationmode=sample-hold);
    @statistic[foo0](source=foo2; record=vector); // unitless like foo2, but without a title
    @statistic[foo2](source=foo2; record=mean,last,stats,histogram,vector; title="A very nice foo.");
    @statistic[zero](source=zero; record=mean,last,stats,histogram,vector; unit=pk; title="Zero");
    @statistic[foo3](source=foo2; record=vector; title=""); // unitless like foo2, but with an empty title
}

network Test
{
    submodules:
        node1: Node;
        node2: Node;
}

