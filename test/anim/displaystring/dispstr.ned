import "colors";
import "icons";
import "active";
import "textandtooltip";
import "conntextandtooltip";
import "nedparams";
import "background";

module DisplayStringTest
    submodules:
        colors: Colors;
        icons: Icons;
        active: Active;
        textAndTooltip: TextAndTooltip;
        connTextAndTooltip: ConnTextAndTooltip;
        connColors: ConnColors;
        nedParams: NedParams;
        background: Background;
endmodule

network displayStringTest : DisplayStringTest
endnetwork

