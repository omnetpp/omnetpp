import "dummy";
import "icons";

module TextAndTooltip
    submodules:
        icon_plus_txt: Dummy;
            display: "p=284,340;i=block/sink;t=some lines\nof text";
        icon_plus_txt_left: Dummy;
            display: "p=585,340;i=block/sink;t=some lines\nof text,l";
        icon_plus_txt_right: Dummy;
            display: "p=378,340;i=block/sink;t=some lines\nof text,r";
        icon_plus_txt_top: Dummy;
            display: "p=488,340;i=block/sink;t=some lines\nof text,t";
        tooltip_1: Dummy;
            display: "p=82,360;b=46,19;tt=This is a\nmulti-line tooltip.";
        tooltip_2: Dummy;
            display: "p=163,362;i=block/network;tt=This is a\nsecond tooltip.";
        conn_plus_txt_1_src: Dummy;
            display: "b=40,24;p=82,439";
        conn_plus_txt_1_dest: Dummy;
            display: "b=40,24;p=231,439";
    connections nocheck:
        conn_plus_txt_1_src.out++ --> conn_plus_txt_1_dest.in++ display "t=some text";
endmodule
