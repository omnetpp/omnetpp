import "colors";
import "icons";
import "textandtooltip";
import "conntextandtooltip";

module DisplayStringTest
    submodules:
        colors: Colors;
        icons: Icons;
        active: Active;
        textAndTooltip: TextAndTooltip;
        connTextAndTooltip: ConnTextAndTooltip;
endmodule

network displayStringTest : DisplayStringTest
endnetwork

