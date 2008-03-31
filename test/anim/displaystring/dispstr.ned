network DisplayStringTest
{
    @display("bgb=377,329");
    submodules:
        rangeindicator: rangeindicator {
            @display("p=112,200");
        }
        colors: Colors {
            @display("p=112,130");
        }
        icons: Icons {
            @display("p=35,130");
        }
        textAndTooltip: TextAndTooltip {
            @display("p=272,130");
        }
        connTextAndTooltip: ConnTextAndTooltip {
            @display("p=272,200");
        }
        connColors: ConnColors {
            @display("p=191,130");
        }
        background: Background {
            @display("p=35,200");
        }
        nedParams: NedParams {
            @display("p=112,47");
        }
        scaling: scaling {
            @display("p=191,200");
        }
}


