import "colors";
import "icons";
import "textandtooltip";

module DisplayStringTest
    submodules:
        colors: Colors;
        icons: Icons;
        active: Active;
        textAndTooltip: TextAndTooltip;
endmodule

network displayStringTest : DisplayStringTest
endnetwork

