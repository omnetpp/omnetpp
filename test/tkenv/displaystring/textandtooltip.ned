import
    "dummy";
import
    "icons";

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
        conn_plus_txt_1_src: Dummy;
            display: "i=place/circle2_s;p=95,181";
        conn_plus_txt_1_dest: Dummy;
            display: "i=place/circle2_s;p=243,181";
        conn_plus_txt_2_src: Dummy;
            display: "i=place/circle2_s;p=243,247";
        conn_plus_txt_2_dest: Dummy;
            display: "i=place/circle2_s;p=95,247";
        conn_plus_txt_3_src: Dummy;
            display: "i=place/circle2_s;p=366,183";
        conn_plus_txt_3_dest: Dummy;
            display: "i=place/circle2_s;p=366,293";
        conn_plus_txt_4_src: Dummy;
            display: "i=place/circle2_s;p=488,292";
        conn_plus_txt_4_dest: Dummy;
            display: "i=place/circle2_s;p=488,181";
        conn_plus_txt_5_src: Dummy;
            display: "i=place/circle2_s;p=74,456";
        conn_plus_txt_5_dest: Dummy;
            display: "i=place/circle2_s;p=193,368";
        conn_plus_txt_6_src: Dummy;
            display: "i=place/circle2_s;p=302,367";
        conn_plus_txt_6_dest: Dummy;
            display: "i=place/circle2_s;p=184,457";
        conn_plus_txt_7_src: Dummy;
            display: "i=place/circle2_s;p=560,438";
        conn_plus_txt_7_dest: Dummy;
            display: "i=place/circle2_s;p=411,367";
        conn_plus_txt_8_src: Dummy;
            display: "i=place/circle2_s;p=527,364";
        conn_plus_txt_8_dest: Dummy;
            display: "i=place/circle2_s;p=675,436";
    connections nocheck:
        conn_plus_txt_1_src.out++ --> conn_plus_txt_1_dest.in++ display "t=some text;tt=some tooltip";
        conn_plus_txt_2_src.out++ --> conn_plus_txt_2_dest.in++ display "t=some text;tt=some tooltip";
        conn_plus_txt_3_src.out++ --> conn_plus_txt_3_dest.in++ display "t=some text;tt=some tooltip";
        conn_plus_txt_4_src.out++ --> conn_plus_txt_4_dest.in++ display "t=some text;tt=some tooltip";
        conn_plus_txt_5_src.out++ --> conn_plus_txt_5_dest.in++ display "t=some text;tt=some tooltip";
        conn_plus_txt_6_src.out++ --> conn_plus_txt_6_dest.in++ display "t=some text;tt=some tooltip";
        conn_plus_txt_7_src.out++ --> conn_plus_txt_7_dest.in++ display "t=some text;tt=some tooltip";
        conn_plus_txt_8_src.out++ --> conn_plus_txt_8_dest.in++ display "t=some text;tt=some tooltip";
endmodule

