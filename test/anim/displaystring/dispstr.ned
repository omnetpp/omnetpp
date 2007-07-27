import "colors";
import "icons";
import "active";
import "textandtooltip";
import "conntextandtooltip";
import "nedparams";
import "background";

network DisplayStringTest
{
    @display("bgb=377,329");
    submodules:
        colors: Colors {
            @display("p=112,130");
        }
        icons: Icons {
            @display("p=35,130");
        }
        textAndTooltip: TextAndTooltip {
            @display("p=192,130");
        }
        connTextAndTooltip: ConnTextAndTooltip {
            @display("p=192,200");
        }
        connColors: ConnColors {
            @display("p=272,130");
        }
        background: Background {
            @display("p=35,200");
        }
        nedParams: NedParams {
            @display("p=112,47");
        }
//        active: Active {
//            @display("p=36,47");
//        }
}


