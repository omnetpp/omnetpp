import
    "dummy";

module ConnTextAndTooltip
    submodules:
        conn_plus_txt_1_src: Dummy;
            display: "i=place/circle2_s;p=93,72";
        conn_plus_txt_1_dest: Dummy;
            display: "i=place/circle2_s;p=241,72";
        conn_plus_txt_2_src: Dummy;
            display: "i=place/circle2_s;p=241,138";
        conn_plus_txt_2_dest: Dummy;
            display: "i=place/circle2_s;p=93,138";
        conn_plus_txt_3_src: Dummy;
            display: "i=place/circle2_s;p=364,74";
        conn_plus_txt_3_dest: Dummy;
            display: "i=place/circle2_s;p=364,184";
        conn_plus_txt_4_src: Dummy;
            display: "i=place/circle2_s;p=486,183";
        conn_plus_txt_4_dest: Dummy;
            display: "i=place/circle2_s;p=486,72";
        conn_plus_txt_5_src: Dummy;
            display: "i=place/circle2_s;p=72,347";
        conn_plus_txt_5_dest: Dummy;
            display: "i=place/circle2_s;p=191,259";
        conn_plus_txt_6_src: Dummy;
            display: "i=place/circle2_s;p=300,258";
        conn_plus_txt_6_dest: Dummy;
            display: "i=place/circle2_s;p=182,348";
        conn_plus_txt_7_src: Dummy;
            display: "i=place/circle2_s;p=558,329";
        conn_plus_txt_7_dest: Dummy;
            display: "i=place/circle2_s;p=409,258";
        conn_plus_txt_8_src: Dummy;
            display: "i=place/circle2_s;p=525,255";
        conn_plus_txt_8_dest: Dummy;
            display: "i=place/circle2_s;p=673,327";
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

