import "colors";
import "icons";
import "textandtooltip";
import "conntextandtooltip";
import "nedparams";

module DisplayStringTest
    submodules:
        colors: Colors;
        icons: Icons;
        active: Active;
        textAndTooltip: TextAndTooltip;
        connTextAndTooltip: ConnTextAndTooltip;
        nedParams: NedParams;
endmodule

network displayStringTest : DisplayStringTest
endnetwork

