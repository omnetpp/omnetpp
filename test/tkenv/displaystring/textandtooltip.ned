import "dummy";
import "icons";

module TextAndTooltip
    submodules:
        icon_plus_txt: Dummy;
            display: "p=70,89;i=block/sink;t=some lines\nof text";
        icon_plus_txt_left: Dummy;
            display: "p=371,89;i=block/sink;t=some lines\nof text,l";
        icon_plus_txt_right: Dummy;
            display: "p=164,89;i=block/sink;t=some lines\nof text,r";
        icon_plus_txt_top: Dummy;
            display: "p=274,89;i=block/sink;t=some lines\nof text,t";
        tooltip_1: Dummy;
            display: "p=488,90;b=46,18;tt=This is a\nmulti-line tooltip.";
        tooltip_2: Dummy;
            display: "p=559,93;i=block/network;tt=This is a\nsecond tooltip.";
endmodule

