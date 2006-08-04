package org.omnetpp.common.color;

import java.util.HashMap;

import org.eclipse.jface.resource.ColorRegistry;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.RGB;

// TODO HSL support should be added (@HHSSLL format)
public class ColorFactory {
    public final static Color highlight = new Color(null, 255, 0, 0);
    public final static Color defaultBackground = new Color(null, 192, 192, 192);
    public final static Color defaultBorder = new Color(null, 0, 0, 0);

    private static ColorRegistry str2rgbRegistry = new ColorRegistry();
    private static HashMap<RGB, String> rgb2strMap = new HashMap<RGB, String>();
    private static Color[] goodChartColors; 
    
    static {
        addMapping("antiqueWhite",new RGB(250,235,215));
        addMapping("antiqueWhite1",new RGB(255,239,219));
        addMapping("antiqueWhite2",new RGB(238,223,204));
        addMapping("antiqueWhite3",new RGB(205,192,176));
        addMapping("antiqueWhite4",new RGB(139,131,120));
        addMapping("blanchedAlmond",new RGB(255,235,205));
        addMapping("blueViolet",new RGB(138,43,226));
        addMapping("cadetBlue",new RGB(95,158,160));
        addMapping("cadetBlue1",new RGB(152,245,255));
        addMapping("cadetBlue2",new RGB(142,229,238));
        addMapping("cadetBlue3",new RGB(122,197,205));
        addMapping("cadetBlue4",new RGB(83,134,139));
        addMapping("cornflowerBlue",new RGB(100,149,237));
        addMapping("darkBlue",new RGB(0,0,139));
        addMapping("darkCyan",new RGB(0,139,139));
        addMapping("darkGoldenrod",new RGB(184,134,11));
        addMapping("darkGoldenrod1",new RGB(255,185,15));
        addMapping("darkGoldenrod2",new RGB(238,173,14));
        addMapping("darkGoldenrod3",new RGB(205,149,12));
        addMapping("darkGoldenrod4",new RGB(139,101,8));
        addMapping("darkGreen",new RGB(0,100,0));
        addMapping("darkGrey",new RGB(169,169,169));
        addMapping("darkKhaki",new RGB(189,183,107));
        addMapping("darkMagenta",new RGB(139,0,139));
        addMapping("darkOliveGreen",new RGB(85,107,47));
        addMapping("darkOliveGreen1",new RGB(202,255,112));
        addMapping("darkOliveGreen2",new RGB(188,238,104));
        addMapping("darkOliveGreen3",new RGB(162,205,90));
        addMapping("darkOliveGreen4",new RGB(110,139,61));
        addMapping("darkOrange",new RGB(255,140,0));
        addMapping("darkOrange1",new RGB(255,127,0));
        addMapping("darkOrange2",new RGB(238,118,0));
        addMapping("darkOrange3",new RGB(205,102,0));
        addMapping("darkOrange4",new RGB(139,69,0));
        addMapping("darkOrchid",new RGB(153,50,204));
        addMapping("darkOrchid1",new RGB(191,62,255));
        addMapping("darkOrchid2",new RGB(178,58,238));
        addMapping("darkOrchid3",new RGB(154,50,205));
        addMapping("darkOrchid4",new RGB(104,34,139));
        addMapping("darkRed",new RGB(139,0,0));
        addMapping("darkSalmon",new RGB(233,150,122));
        addMapping("darkSeaGreen",new RGB(143,188,143));
        addMapping("darkSeaGreen1",new RGB(193,255,193));
        addMapping("darkSeaGreen2",new RGB(180,238,180));
        addMapping("darkSeaGreen3",new RGB(155,205,155));
        addMapping("darkSeaGreen4",new RGB(105,139,105));
        addMapping("darkSlateBlue",new RGB(72,61,139));
        addMapping("darkSlateGrey",new RGB(47,79,79));
        addMapping("darkTurquoise",new RGB(0,206,209));
        addMapping("darkViolet",new RGB(148,0,211));
        addMapping("deepPink",new RGB(255,20,147));
        addMapping("deepPink1",new RGB(255,20,147));
        addMapping("deepPink2",new RGB(238,18,137));
        addMapping("deepPink3",new RGB(205,16,118));
        addMapping("deepPink4",new RGB(139,10,80));
        addMapping("deepSkyBlue",new RGB(0,191,255));
        addMapping("deepSkyBlue1",new RGB(0,191,255));
        addMapping("deepSkyBlue2",new RGB(0,178,238));
        addMapping("deepSkyBlue3",new RGB(0,154,205));
        addMapping("deepSkyBlue4",new RGB(0,104,139));
        addMapping("dimGrey",new RGB(105,105,105));
        addMapping("dodgerBlue",new RGB(30,144,255));
        addMapping("dodgerBlue1",new RGB(30,144,255));
        addMapping("dodgerBlue2",new RGB(28,134,238));
        addMapping("dodgerBlue3",new RGB(24,116,205));
        addMapping("dodgerBlue4",new RGB(16,78,139));
        addMapping("floralWhite",new RGB(255,250,240));
        addMapping("forestGreen",new RGB(34,139,34));
        addMapping("ghostWhite",new RGB(248,248,255));
        addMapping("greenYellow",new RGB(173,255,47));
        addMapping("hotPink",new RGB(255,105,180));
        addMapping("hotPink1",new RGB(255,110,180));
        addMapping("hotPink2",new RGB(238,106,167));
        addMapping("hotPink3",new RGB(205,96,144));
        addMapping("hotPink4",new RGB(139,58,98));
        addMapping("indianRed",new RGB(205,92,92));
        addMapping("indianRed1",new RGB(255,106,106));
        addMapping("indianRed2",new RGB(238,99,99));
        addMapping("indianRed3",new RGB(205,85,85));
        addMapping("indianRed4",new RGB(139,58,58));
        addMapping("lavenderBlush",new RGB(255,240,245));
        addMapping("lavenderBlush1",new RGB(255,240,245));
        addMapping("lavenderBlush2",new RGB(238,224,229));
        addMapping("lavenderBlush3",new RGB(205,193,197));
        addMapping("lavenderBlush4",new RGB(139,131,134));
        addMapping("lawnGreen",new RGB(124,252,0));
        addMapping("lemonChiffon",new RGB(255,250,205));
        addMapping("lemonChiffon1",new RGB(255,250,205));
        addMapping("lemonChiffon2",new RGB(238,233,191));
        addMapping("lemonChiffon3",new RGB(205,201,165));
        addMapping("lemonChiffon4",new RGB(139,137,112));
        addMapping("lightBlue",new RGB(173,216,230));
        addMapping("lightBlue1",new RGB(191,239,255));
        addMapping("lightBlue2",new RGB(178,223,238));
        addMapping("lightBlue3",new RGB(154,192,205));
        addMapping("lightBlue4",new RGB(104,131,139));
        addMapping("lightCoral",new RGB(240,128,128));
        addMapping("lightCyan",new RGB(224,255,255));
        addMapping("lightCyan1",new RGB(224,255,255));
        addMapping("lightCyan2",new RGB(209,238,238));
        addMapping("lightCyan3",new RGB(180,205,205));
        addMapping("lightCyan4",new RGB(122,139,139));
        addMapping("lightGoldenrod",new RGB(238,221,130));
        addMapping("lightGoldenrod1",new RGB(255,236,139));
        addMapping("lightGoldenrod2",new RGB(238,220,130));
        addMapping("lightGoldenrod3",new RGB(205,190,112));
        addMapping("lightGoldenrod4",new RGB(139,129,76));
        addMapping("lightGoldenrodYellow",new RGB(250,250,210));
        addMapping("lightGreen",new RGB(144,238,144));
        addMapping("lightGrey",new RGB(211,211,211));
        addMapping("lightPink",new RGB(255,182,193));
        addMapping("lightPink1",new RGB(255,174,185));
        addMapping("lightPink2",new RGB(238,162,173));
        addMapping("lightPink3",new RGB(205,140,149));
        addMapping("lightPink4",new RGB(139,95,101));
        addMapping("lightSalmon",new RGB(255,160,122));
        addMapping("lightSalmon1",new RGB(255,160,122));
        addMapping("lightSalmon2",new RGB(238,149,114));
        addMapping("lightSalmon3",new RGB(205,129,98));
        addMapping("lightSalmon4",new RGB(139,87,66));
        addMapping("lightSeaGreen",new RGB(32,178,170));
        addMapping("lightSkyBlue",new RGB(135,206,250));
        addMapping("lightSkyBlue1",new RGB(176,226,255));
        addMapping("lightSkyBlue2",new RGB(164,211,238));
        addMapping("lightSkyBlue3",new RGB(141,182,205));
        addMapping("lightSkyBlue4",new RGB(96,123,139));
        addMapping("lightSlateBlue",new RGB(132,112,255));
        addMapping("lightSlateGrey",new RGB(119,136,153));
        addMapping("lightSteelBlue",new RGB(176,196,222));
        addMapping("lightSteelBlue1",new RGB(202,225,255));
        addMapping("lightSteelBlue2",new RGB(188,210,238));
        addMapping("lightSteelBlue3",new RGB(162,181,205));
        addMapping("lightSteelBlue4",new RGB(110,123,139));
        addMapping("lightYellow",new RGB(255,255,224));
        addMapping("lightYellow1",new RGB(255,255,224));
        addMapping("lightYellow2",new RGB(238,238,209));
        addMapping("lightYellow3",new RGB(205,205,180));
        addMapping("lightYellow4",new RGB(139,139,122));
        addMapping("limeGreen",new RGB(50,205,50));
        addMapping("mediumAquamarine",new RGB(102,205,170));
        addMapping("mediumBlue",new RGB(0,0,205));
        addMapping("mediumOrchid",new RGB(186,85,211));
        addMapping("mediumOrchid1",new RGB(224,102,255));
        addMapping("mediumOrchid2",new RGB(209,95,238));
        addMapping("mediumOrchid3",new RGB(180,82,205));
        addMapping("mediumOrchid4",new RGB(122,55,139));
        addMapping("mediumPurple",new RGB(147,112,219));
        addMapping("mediumPurple1",new RGB(171,130,255));
        addMapping("mediumPurple2",new RGB(159,121,238));
        addMapping("mediumPurple3",new RGB(137,104,205));
        addMapping("mediumPurple4",new RGB(93,71,139));
        addMapping("mediumSeaGreen",new RGB(60,179,113));
        addMapping("mediumSlateBlue",new RGB(123,104,238));
        addMapping("mediumSpringGreen",new RGB(0,250,154));
        addMapping("mediumTurquoise",new RGB(72,209,204));
        addMapping("mediumVioletRed",new RGB(199,21,133));
        addMapping("midnightBlue",new RGB(25,25,112));
        addMapping("mintCream",new RGB(245,255,250));
        addMapping("mistyRose",new RGB(255,228,225));
        addMapping("mistyRose1",new RGB(255,228,225));
        addMapping("mistyRose2",new RGB(238,213,210));
        addMapping("mistyRose3",new RGB(205,183,181));
        addMapping("mistyRose4",new RGB(139,125,123));
        addMapping("navajoWhite",new RGB(255,222,173));
        addMapping("navajoWhite1",new RGB(255,222,173));
        addMapping("navajoWhite2",new RGB(238,207,161));
        addMapping("navajoWhite3",new RGB(205,179,139));
        addMapping("navajoWhite4",new RGB(139,121,94));
        addMapping("navyBlue",new RGB(0,0,128));
        addMapping("oldLace",new RGB(253,245,230));
        addMapping("oliveDrab",new RGB(107,142,35));
        addMapping("oliveDrab1",new RGB(192,255,62));
        addMapping("oliveDrab2",new RGB(179,238,58));
        addMapping("oliveDrab3",new RGB(154,205,50));
        addMapping("oliveDrab4",new RGB(105,139,34));
        addMapping("orangeRed",new RGB(255,69,0));
        addMapping("orangeRed1",new RGB(255,69,0));
        addMapping("orangeRed2",new RGB(238,64,0));
        addMapping("orangeRed3",new RGB(205,55,0));
        addMapping("orangeRed4",new RGB(139,37,0));
        addMapping("paleGoldenrod",new RGB(238,232,170));
        addMapping("paleGreen",new RGB(152,251,152));
        addMapping("paleGreen1",new RGB(154,255,154));
        addMapping("paleGreen2",new RGB(144,238,144));
        addMapping("paleGreen3",new RGB(124,205,124));
        addMapping("paleGreen4",new RGB(84,139,84));
        addMapping("paleTurquoise",new RGB(175,238,238));
        addMapping("paleTurquoise1",new RGB(187,255,255));
        addMapping("paleTurquoise2",new RGB(174,238,238));
        addMapping("paleTurquoise3",new RGB(150,205,205));
        addMapping("paleTurquoise4",new RGB(102,139,139));
        addMapping("paleVioletRed",new RGB(219,112,147));
        addMapping("paleVioletRed1",new RGB(255,130,171));
        addMapping("paleVioletRed2",new RGB(238,121,159));
        addMapping("paleVioletRed3",new RGB(205,104,137));
        addMapping("paleVioletRed4",new RGB(139,71,93));
        addMapping("papayaWhip",new RGB(255,239,213));
        addMapping("peachPuff",new RGB(255,218,185));
        addMapping("peachPuff1",new RGB(255,218,185));
        addMapping("peachPuff2",new RGB(238,203,173));
        addMapping("peachPuff3",new RGB(205,175,149));
        addMapping("peachPuff4",new RGB(139,119,101));
        addMapping("powderBlue",new RGB(176,224,230));
        addMapping("rosyBrown",new RGB(188,143,143));
        addMapping("rosyBrown1",new RGB(255,193,193));
        addMapping("rosyBrown2",new RGB(238,180,180));
        addMapping("rosyBrown3",new RGB(205,155,155));
        addMapping("rosyBrown4",new RGB(139,105,105));
        addMapping("royalBlue",new RGB(65,105,225));
        addMapping("royalBlue1",new RGB(72,118,255));
        addMapping("royalBlue2",new RGB(67,110,238));
        addMapping("royalBlue3",new RGB(58,95,205));
        addMapping("royalBlue4",new RGB(39,64,139));
        addMapping("saddleBrown",new RGB(139,69,19));
        addMapping("sandyBrown",new RGB(244,164,96));
        addMapping("seaGreen",new RGB(46,139,87));
        addMapping("seaGreen1",new RGB(84,255,159));
        addMapping("seaGreen2",new RGB(78,238,148));
        addMapping("seaGreen3",new RGB(67,205,128));
        addMapping("seaGreen4",new RGB(46,139,87));
        addMapping("skyBlue",new RGB(135,206,235));
        addMapping("skyBlue1",new RGB(135,206,255));
        addMapping("skyBlue2",new RGB(126,192,238));
        addMapping("skyBlue3",new RGB(108,166,205));
        addMapping("skyBlue4",new RGB(74,112,139));
        addMapping("slateBlue",new RGB(106,90,205));
        addMapping("slateBlue1",new RGB(131,111,255));
        addMapping("slateBlue2",new RGB(122,103,238));
        addMapping("slateBlue3",new RGB(105,89,205));
        addMapping("slateBlue4",new RGB(71,60,139));
        addMapping("slateGrey",new RGB(112,128,144));
        addMapping("springGreen",new RGB(0,255,127));
        addMapping("springGreen1",new RGB(0,255,127));
        addMapping("springGreen2",new RGB(0,238,118));
        addMapping("springGreen3",new RGB(0,205,102));
        addMapping("springGreen4",new RGB(0,139,69));
        addMapping("steelBlue",new RGB(70,130,180));
        addMapping("steelBlue1",new RGB(99,184,255));
        addMapping("steelBlue2",new RGB(92,172,238));
        addMapping("steelBlue3",new RGB(79,148,205));
        addMapping("steelBlue4",new RGB(54,100,139));
        addMapping("violetRed",new RGB(208,32,144));
        addMapping("violetRed1",new RGB(255,62,150));
        addMapping("violetRed2",new RGB(238,58,140));
        addMapping("violetRed3",new RGB(205,50,120));
        addMapping("violetRed4",new RGB(139,34,82));
        addMapping("whiteSmoke",new RGB(245,245,245));
        addMapping("yellowGreen",new RGB(154,205,50));
        addMapping("aquamarine",new RGB(127,255,212));
        addMapping("aquamarine1",new RGB(127,255,212));
        addMapping("aquamarine2",new RGB(118,238,198));
        addMapping("aquamarine3",new RGB(102,205,170));
        addMapping("aquamarine4",new RGB(69,139,116));
        addMapping("azure",new RGB(240,255,255));
        addMapping("azure1",new RGB(240,255,255));
        addMapping("azure2",new RGB(224,238,238));
        addMapping("azure3",new RGB(193,205,205));
        addMapping("azure4",new RGB(131,139,139));
        addMapping("beige",new RGB(245,245,220));
        addMapping("bisque",new RGB(255,228,196));
        addMapping("bisque1",new RGB(255,228,196));
        addMapping("bisque2",new RGB(238,213,183));
        addMapping("bisque3",new RGB(205,183,158));
        addMapping("bisque4",new RGB(139,125,107));
        addMapping("black",new RGB(0,0,0));
        addMapping("blue",new RGB(0,0,255));
        addMapping("blue1",new RGB(0,0,255));
        addMapping("blue2",new RGB(0,0,238));
        addMapping("blue3",new RGB(0,0,205));
        addMapping("blue4",new RGB(0,0,139));
        addMapping("brown",new RGB(165,42,42));
        addMapping("brown1",new RGB(255,64,64));
        addMapping("brown2",new RGB(238,59,59));
        addMapping("brown3",new RGB(205,51,51));
        addMapping("brown4",new RGB(139,35,35));
        addMapping("burlywood",new RGB(222,184,135));
        addMapping("burlywood1",new RGB(255,211,155));
        addMapping("burlywood2",new RGB(238,197,145));
        addMapping("burlywood3",new RGB(205,170,125));
        addMapping("burlywood4",new RGB(139,115,85));
        addMapping("chartreuse",new RGB(127,255,0));
        addMapping("chartreuse1",new RGB(127,255,0));
        addMapping("chartreuse2",new RGB(118,238,0));
        addMapping("chartreuse3",new RGB(102,205,0));
        addMapping("chartreuse4",new RGB(69,139,0));
        addMapping("chocolate",new RGB(210,105,30));
        addMapping("chocolate1",new RGB(255,127,36));
        addMapping("chocolate2",new RGB(238,118,33));
        addMapping("chocolate3",new RGB(205,102,29));
        addMapping("chocolate4",new RGB(139,69,19));
        addMapping("coral",new RGB(255,127,80));
        addMapping("coral1",new RGB(255,114,86));
        addMapping("coral2",new RGB(238,106,80));
        addMapping("coral3",new RGB(205,91,69));
        addMapping("coral4",new RGB(139,62,47));
        addMapping("cornsilk",new RGB(255,248,220));
        addMapping("cornsilk1",new RGB(255,248,220));
        addMapping("cornsilk2",new RGB(238,232,205));
        addMapping("cornsilk3",new RGB(205,200,177));
        addMapping("cornsilk4",new RGB(139,136,120));
        addMapping("cyan",new RGB(0,255,255));
        addMapping("cyan1",new RGB(0,255,255));
        addMapping("cyan2",new RGB(0,238,238));
        addMapping("cyan3",new RGB(0,205,205));
        addMapping("cyan4",new RGB(0,139,139));
        addMapping("firebrick",new RGB(178,34,34));
        addMapping("firebrick1",new RGB(255,48,48));
        addMapping("firebrick2",new RGB(238,44,44));
        addMapping("firebrick3",new RGB(205,38,38));
        addMapping("firebrick4",new RGB(139,26,26));
        addMapping("gainsboro",new RGB(220,220,220));
        addMapping("gold",new RGB(255,215,0));
        addMapping("gold1",new RGB(255,215,0));
        addMapping("gold2",new RGB(238,201,0));
        addMapping("gold3",new RGB(205,173,0));
        addMapping("gold4",new RGB(139,117,0));
        addMapping("goldenrod",new RGB(218,165,32));
        addMapping("goldenrod1",new RGB(255,193,37));
        addMapping("goldenrod2",new RGB(238,180,34));
        addMapping("goldenrod3",new RGB(205,155,29));
        addMapping("goldenrod4",new RGB(139,105,20));
        addMapping("green",new RGB(0,255,0));
        addMapping("green1",new RGB(0,255,0));
        addMapping("green2",new RGB(0,238,0));
        addMapping("green3",new RGB(0,205,0));
        addMapping("green4",new RGB(0,139,0));
        addMapping("grey",new RGB(192,192,192));
        addMapping("grey0",new RGB(0,0,0));
        addMapping("grey1",new RGB(3,3,3));
        addMapping("grey10",new RGB(26,26,26));
        addMapping("grey100",new RGB(255,255,255));
        addMapping("grey11",new RGB(28,28,28));
        addMapping("grey12",new RGB(31,31,31));
        addMapping("grey13",new RGB(33,33,33));
        addMapping("grey14",new RGB(36,36,36));
        addMapping("grey15",new RGB(38,38,38));
        addMapping("grey16",new RGB(41,41,41));
        addMapping("grey17",new RGB(43,43,43));
        addMapping("grey18",new RGB(46,46,46));
        addMapping("grey19",new RGB(48,48,48));
        addMapping("grey2",new RGB(5,5,5));
        addMapping("grey20",new RGB(51,51,51));
        addMapping("grey21",new RGB(54,54,54));
        addMapping("grey22",new RGB(56,56,56));
        addMapping("grey23",new RGB(59,59,59));
        addMapping("grey24",new RGB(61,61,61));
        addMapping("grey25",new RGB(64,64,64));
        addMapping("grey26",new RGB(66,66,66));
        addMapping("grey27",new RGB(69,69,69));
        addMapping("grey28",new RGB(71,71,71));
        addMapping("grey29",new RGB(74,74,74));
        addMapping("grey3",new RGB(8,8,8));
        addMapping("grey30",new RGB(77,77,77));
        addMapping("grey31",new RGB(79,79,79));
        addMapping("grey32",new RGB(82,82,82));
        addMapping("grey33",new RGB(84,84,84));
        addMapping("grey34",new RGB(87,87,87));
        addMapping("grey35",new RGB(89,89,89));
        addMapping("grey36",new RGB(92,92,92));
        addMapping("grey37",new RGB(94,94,94));
        addMapping("grey38",new RGB(97,97,97));
        addMapping("grey39",new RGB(99,99,99));
        addMapping("grey4",new RGB(10,10,10));
        addMapping("grey40",new RGB(102,102,102));
        addMapping("grey41",new RGB(105,105,105));
        addMapping("grey42",new RGB(107,107,107));
        addMapping("grey43",new RGB(110,110,110));
        addMapping("grey44",new RGB(112,112,112));
        addMapping("grey45",new RGB(115,115,115));
        addMapping("grey46",new RGB(117,117,117));
        addMapping("grey47",new RGB(120,120,120));
        addMapping("grey48",new RGB(122,122,122));
        addMapping("grey49",new RGB(125,125,125));
        addMapping("grey5",new RGB(13,13,13));
        addMapping("grey50",new RGB(127,127,127));
        addMapping("grey51",new RGB(130,130,130));
        addMapping("grey52",new RGB(133,133,133));
        addMapping("grey53",new RGB(135,135,135));
        addMapping("grey54",new RGB(138,138,138));
        addMapping("grey55",new RGB(140,140,140));
        addMapping("grey56",new RGB(143,143,143));
        addMapping("grey57",new RGB(145,145,145));
        addMapping("grey58",new RGB(148,148,148));
        addMapping("grey59",new RGB(150,150,150));
        addMapping("grey6",new RGB(15,15,15));
        addMapping("grey60",new RGB(153,153,153));
        addMapping("grey61",new RGB(156,156,156));
        addMapping("grey62",new RGB(158,158,158));
        addMapping("grey63",new RGB(161,161,161));
        addMapping("grey64",new RGB(163,163,163));
        addMapping("grey65",new RGB(166,166,166));
        addMapping("grey66",new RGB(168,168,168));
        addMapping("grey67",new RGB(171,171,171));
        addMapping("grey68",new RGB(173,173,173));
        addMapping("grey69",new RGB(176,176,176));
        addMapping("grey7",new RGB(18,18,18));
        addMapping("grey70",new RGB(179,179,179));
        addMapping("grey71",new RGB(181,181,181));
        addMapping("grey72",new RGB(184,184,184));
        addMapping("grey73",new RGB(186,186,186));
        addMapping("grey74",new RGB(189,189,189));
        addMapping("grey75",new RGB(191,191,191));
        addMapping("grey76",new RGB(194,194,194));
        addMapping("grey77",new RGB(196,196,196));
        addMapping("grey78",new RGB(199,199,199));
        addMapping("grey79",new RGB(201,201,201));
        addMapping("grey8",new RGB(20,20,20));
        addMapping("grey80",new RGB(204,204,204));
        addMapping("grey81",new RGB(207,207,207));
        addMapping("grey82",new RGB(209,209,209));
        addMapping("grey83",new RGB(212,212,212));
        addMapping("grey84",new RGB(214,214,214));
        addMapping("grey85",new RGB(217,217,217));
        addMapping("grey86",new RGB(219,219,219));
        addMapping("grey87",new RGB(222,222,222));
        addMapping("grey88",new RGB(224,224,224));
        addMapping("grey89",new RGB(227,227,227));
        addMapping("grey9",new RGB(23,23,23));
        addMapping("grey90",new RGB(229,229,229));
        addMapping("grey91",new RGB(232,232,232));
        addMapping("grey92",new RGB(235,235,235));
        addMapping("grey93",new RGB(237,237,237));
        addMapping("grey94",new RGB(240,240,240));
        addMapping("grey95",new RGB(242,242,242));
        addMapping("grey96",new RGB(245,245,245));
        addMapping("grey97",new RGB(247,247,247));
        addMapping("grey98",new RGB(250,250,250));
        addMapping("grey99",new RGB(252,252,252));
        addMapping("honeydew",new RGB(240,255,240));
        addMapping("honeydew1",new RGB(240,255,240));
        addMapping("honeydew2",new RGB(224,238,224));
        addMapping("honeydew3",new RGB(193,205,193));
        addMapping("honeydew4",new RGB(131,139,131));
        addMapping("ivory",new RGB(255,255,240));
        addMapping("ivory1",new RGB(255,255,240));
        addMapping("ivory2",new RGB(238,238,224));
        addMapping("ivory3",new RGB(205,205,193));
        addMapping("ivory4",new RGB(139,139,131));
        addMapping("khaki",new RGB(240,230,140));
        addMapping("khaki1",new RGB(255,246,143));
        addMapping("khaki2",new RGB(238,230,133));
        addMapping("khaki3",new RGB(205,198,115));
        addMapping("khaki4",new RGB(139,134,78));
        addMapping("lavender",new RGB(230,230,250));
        addMapping("linen",new RGB(250,240,230));
        addMapping("magenta",new RGB(255,0,255));
        addMapping("magenta1",new RGB(255,0,255));
        addMapping("magenta2",new RGB(238,0,238));
        addMapping("magenta3",new RGB(205,0,205));
        addMapping("magenta4",new RGB(139,0,139));
        addMapping("maroon",new RGB(176,48,96));
        addMapping("maroon1",new RGB(255,52,179));
        addMapping("maroon2",new RGB(238,48,167));
        addMapping("maroon3",new RGB(205,41,144));
        addMapping("maroon4",new RGB(139,28,98));
        addMapping("moccasin",new RGB(255,228,181));
        addMapping("navy",new RGB(0,0,128));
        addMapping("orange",new RGB(255,165,0));
        addMapping("orange1",new RGB(255,165,0));
        addMapping("orange2",new RGB(238,154,0));
        addMapping("orange3",new RGB(205,133,0));
        addMapping("orange4",new RGB(139,90,0));
        addMapping("orchid",new RGB(218,112,214));
        addMapping("orchid1",new RGB(255,131,250));
        addMapping("orchid2",new RGB(238,122,233));
        addMapping("orchid3",new RGB(205,105,201));
        addMapping("orchid4",new RGB(139,71,137));
        addMapping("peru",new RGB(205,133,63));
        addMapping("pink",new RGB(255,192,203));
        addMapping("pink1",new RGB(255,181,197));
        addMapping("pink2",new RGB(238,169,184));
        addMapping("pink3",new RGB(205,145,158));
        addMapping("pink4",new RGB(139,99,108));
        addMapping("plum",new RGB(221,160,221));
        addMapping("plum1",new RGB(255,187,255));
        addMapping("plum2",new RGB(238,174,238));
        addMapping("plum3",new RGB(205,150,205));
        addMapping("plum4",new RGB(139,102,139));
        addMapping("purple",new RGB(160,32,240));
        addMapping("purple1",new RGB(155,48,255));
        addMapping("purple2",new RGB(145,44,238));
        addMapping("purple3",new RGB(125,38,205));
        addMapping("purple4",new RGB(85,26,139));
        addMapping("red",new RGB(255,0,0));
        addMapping("red1",new RGB(255,0,0));
        addMapping("red2",new RGB(238,0,0));
        addMapping("red3",new RGB(205,0,0));
        addMapping("red4",new RGB(139,0,0));
        addMapping("salmon",new RGB(250,128,114));
        addMapping("salmon1",new RGB(255,140,105));
        addMapping("salmon2",new RGB(238,130,98));
        addMapping("salmon3",new RGB(205,112,84));
        addMapping("salmon4",new RGB(139,76,57));
        addMapping("seashell",new RGB(255,245,238));
        addMapping("seashell1",new RGB(255,245,238));
        addMapping("seashell2",new RGB(238,229,222));
        addMapping("seashell3",new RGB(205,197,191));
        addMapping("seashell4",new RGB(139,134,130));
        addMapping("sienna",new RGB(160,82,45));
        addMapping("sienna1",new RGB(255,130,71));
        addMapping("sienna2",new RGB(238,121,66));
        addMapping("sienna3",new RGB(205,104,57));
        addMapping("sienna4",new RGB(139,71,38));
        addMapping("snow",new RGB(255,250,250));
        addMapping("snow1",new RGB(255,250,250));
        addMapping("snow2",new RGB(238,233,233));
        addMapping("snow3",new RGB(205,201,201));
        addMapping("snow4",new RGB(139,137,137));
        addMapping("tan",new RGB(210,180,140));
        addMapping("tan1",new RGB(255,165,79));
        addMapping("tan2",new RGB(238,154,73));
        addMapping("tan3",new RGB(205,133,63));
        addMapping("tan4",new RGB(139,90,43));
        addMapping("thistle",new RGB(216,191,216));
        addMapping("thistle1",new RGB(255,225,255));
        addMapping("thistle2",new RGB(238,210,238));
        addMapping("thistle3",new RGB(205,181,205));
        addMapping("thistle4",new RGB(139,123,139));
        addMapping("tomato",new RGB(255,99,71));
        addMapping("tomato1",new RGB(255,99,71));
        addMapping("tomato2",new RGB(238,92,66));
        addMapping("tomato3",new RGB(205,79,57));
        addMapping("tomato4",new RGB(139,54,38));
        addMapping("turquoise",new RGB(64,224,208));
        addMapping("turquoise1",new RGB(0,245,255));
        addMapping("turquoise2",new RGB(0,229,238));
        addMapping("turquoise3",new RGB(0,197,205));
        addMapping("turquoise4",new RGB(0,134,139));
        addMapping("violet",new RGB(238,130,238));
        addMapping("wheat",new RGB(245,222,179));
        addMapping("wheat1",new RGB(255,231,186));
        addMapping("wheat2",new RGB(238,216,174));
        addMapping("wheat3",new RGB(205,186,150));
        addMapping("wheat4",new RGB(139,126,102));
        addMapping("white",new RGB(255,255,255));
        addMapping("yellow",new RGB(255,255,0));
        addMapping("yellow1",new RGB(255,255,0));
        addMapping("yellow2",new RGB(238,238,0));
        addMapping("yellow3",new RGB(205,205,0));
        addMapping("yellow4",new RGB(139,139,0));

        goodChartColors = new Color[] {
        		ColorFactory.asColor("blue"),
        		ColorFactory.asColor("red2"),
        		ColorFactory.asColor("green"),
        		ColorFactory.asColor("orange"),
        		ColorFactory.asColor("#008000"),
        		ColorFactory.asColor("darkGrey"), 
        		ColorFactory.asColor("#a00000"), 
        		ColorFactory.asColor("#008080"), 
        		ColorFactory.asColor("cyan"), 
        		ColorFactory.asColor("#808000"),
        		ColorFactory.asColor("#8080ff"), 
        		ColorFactory.asColor("yellow"), 
        		ColorFactory.asColor("black"), 
        		ColorFactory.asColor("purple"), 
        		ColorFactory.asColor("grey")
        		//XXX add more
        };
	};

    // helper function to fill the color table
    private static void addMapping(String name, RGB rgb) {
        str2rgbRegistry.put(name.toLowerCase(), rgb);
        // check if the given RGB is already in the map
        String colName = rgb2strMap.get(rgb);
        // if the new colorname is shorter use that for the reverse mapping and remove the previous one
        if (colName == null || colName.length() > name.length()) {
            rgb2strMap.remove(rgb);
            rgb2strMap.put(rgb, name);
        }
    }
    
    /**
     * Returns the color name, or #RRGGBB if no constant found for the color. Throws
     * <code>NullPointerException</code> if the parametr is <code>null</code> 
     * @param value RGB object to be converted
     * @return String representation of the color 
     */
    public static String asString(RGB value) {
        // check for color constatns first
        String constName = rgb2strMap.get(value);
        if(constName != null) return constName;
        
        // if no constant was found, create a hex description
        return String.format("#%06X", value.red << 16 | value.green << 8 | value.blue);
    }

    /**
     * Parses a string into an RGB object. The format can be #RRGGBB @HHSSII or 
     * a color constant name like: red, green
     * Throws null pointer exception on incoming <code>null</code> 
     * @param value Sring to be parsed
     * @return RGB or <code>null</code> on parse error
     */
    public static RGB asRGB(String value) {
        RGB result = null;
        int rgbVal = 0;
        try {
            // check for color constants
            RGB constRGB = str2rgbRegistry.getRGB(value.toLowerCase());
            // return a new RGB object, because RGB is mutable
            if (constRGB != null) 
                return new RGB(constRGB.red, constRGB.green, constRGB.blue);

            // if no constat name found, try to parse as hex string 
            if(value.startsWith("#")) {
                rgbVal = Integer.parseInt(value.substring(1), 16);
                result = new RGB((rgbVal >> 16) & 255, (rgbVal >> 8) & 255, rgbVal & 255);
            }
        } catch (Exception e) {
        }
        return result;
    }
    
    /**
     * Returns a color from a color factory
     * @param value string represtentation of the color
     * @return An SWT color object that can be used or <code>null</code> on parse error
     */
    public static Color asColor(String value) {
        if(value == null) return null;
        // look up in the registry
        Color result = str2rgbRegistry.get(value.toLowerCase());
        // if we have it in the registry, return it
        if (result != null) return result;
        // otherwise create an RGB representation from the string
        RGB tempRgb = asRGB(value);
        // if converison is unsuccessful, return null
        if(tempRgb == null) return null;
        // put it in the registry for later use
        str2rgbRegistry.put(value.toLowerCase(), tempRgb);
        // return it as a Color object
        return str2rgbRegistry.get(value.toLowerCase());
    }

	/**
	 * Returns a "good" color.
	 */
    public static Color getGoodColor(int i) {
		return goodChartColors[i % goodChartColors.length];
	}
    
}
