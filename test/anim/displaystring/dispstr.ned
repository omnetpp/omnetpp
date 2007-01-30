import "colors";
import "icons";
import "active";
import "textandtooltip";
import "conntextandtooltip";
import "nedparams";
import "background";

network DisplayStringTest
{
    submodules:
        colors: Colors;
        icons: Icons;
        active: Active;
        textAndTooltip: TextAndTooltip;
        connTextAndTooltip: ConnTextAndTooltip;
        connColors: ConnColors;
        nedParams: NedParams;
        background: Background;
}


