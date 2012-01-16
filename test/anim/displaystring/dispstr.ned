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
            @display("p=35,40");
        }
        emptyStringParams : EmptyStringParams {
            @display("p=112,49");
        }
        nonemptyStringParams : NonemptyStringParams {
            @display("p=191,40");
        }
        numericParams : NumericParams {
            @display("p=272,49");
        }
        paramsWithBraces : ParamsWithBraces {
            @display("p=350,40");
        }
        scaling: scaling {
            @display("p=191,200");
        }
        scaling2: scaling2 {
            @display("p=35,270");
        }
        active: Active {
            @display("p=112,270");
        }
        connLabelPlacement : ConnLabelPlacement {
            @display("p=191,270");
        }
}


