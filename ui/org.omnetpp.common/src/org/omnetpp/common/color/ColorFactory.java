package org.omnetpp.common.color;

import java.util.HashMap;

import org.eclipse.jface.resource.ColorRegistry;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.resource.ImageRegistry;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.ImageData;
import org.eclipse.swt.graphics.PaletteData;
import org.eclipse.swt.graphics.RGB;

/**
 * A factory that creates and manages colors using symbolic names or
 * #RRGGBB or @HHSSBB format
 * @author rhornig
 */
public class ColorFactory {

    private static ColorRegistry str2rgbRegistry = new ColorRegistry();
    private static HashMap<RGB, String> rgb2strMap = new HashMap<RGB, String>();
    private static ImageRegistry str2img = new ImageRegistry();
    private static Color[] goodDarkColors;
    private static Color[] goodLightColors;

    public static final Color ANTIQUE_WHITE = addMapping("antiqueWhite",new RGB(250,235,215));
    public static final Color ANTIQUE_WHITE1 = addMapping("antiqueWhite1",new RGB(255,239,219));
    public static final Color ANTIQUE_WHITE2 = addMapping("antiqueWhite2",new RGB(238,223,204));
    public static final Color ANTIQUE_WHITE3 = addMapping("antiqueWhite3",new RGB(205,192,176));
    public static final Color ANTIQUE_WHITE4 = addMapping("antiqueWhite4",new RGB(139,131,120));
    public static final Color BLANCHED_ALMOND = addMapping("blanchedAlmond",new RGB(255,235,205));
    public static final Color BLUE_VIOLET = addMapping("blueViolet",new RGB(138,43,226));
    public static final Color CADET_BLUE = addMapping("cadetBlue",new RGB(95,158,160));
    public static final Color CADET_BLUE1 = addMapping("cadetBlue1",new RGB(152,245,255));
    public static final Color CADET_BLUE2 = addMapping("cadetBlue2",new RGB(142,229,238));
    public static final Color CADET_BLUE3 = addMapping("cadetBlue3",new RGB(122,197,205));
    public static final Color CADET_BLUE4 = addMapping("cadetBlue4",new RGB(83,134,139));
    public static final Color CORNFLOWER_BLUE = addMapping("cornflowerBlue",new RGB(100,149,237));
    public static final Color DARK_BLUE = addMapping("darkBlue",new RGB(0,0,139));
    public static final Color DARK_CYAN = addMapping("darkCyan",new RGB(0,139,139));
    public static final Color DARK_GOLDENROD = addMapping("darkGoldenrod",new RGB(184,134,11));
    public static final Color DARK_GOLDENROD1 = addMapping("darkGoldenrod1",new RGB(255,185,15));
    public static final Color DARK_GOLDENROD2 = addMapping("darkGoldenrod2",new RGB(238,173,14));
    public static final Color DARK_GOLDENROD3 = addMapping("darkGoldenrod3",new RGB(205,149,12));
    public static final Color DARK_GOLDENROD4 = addMapping("darkGoldenrod4",new RGB(139,101,8));
    public static final Color DARK_GREEN = addMapping("darkGreen",new RGB(0,100,0));
    public static final Color DARK_GREY = addMapping("darkGrey",new RGB(169,169,169));
    public static final Color DARK_KHAKI = addMapping("darkKhaki",new RGB(189,183,107));
    public static final Color DARK_MAGENTA = addMapping("darkMagenta",new RGB(139,0,139));
    public static final Color DARK_OLIVE_GREEN = addMapping("darkOliveGreen",new RGB(85,107,47));
    public static final Color DARK_OLIVE_GREEN1 = addMapping("darkOliveGreen1",new RGB(202,255,112));
    public static final Color DARK_OLIVE_GREEN2 = addMapping("darkOliveGreen2",new RGB(188,238,104));
    public static final Color DARK_OLIVE_GREEN3 = addMapping("darkOliveGreen3",new RGB(162,205,90));
    public static final Color DARK_OLIVE_GREEN4 = addMapping("darkOliveGreen4",new RGB(110,139,61));
    public static final Color DARK_ORANGE = addMapping("darkOrange",new RGB(255,140,0));
    public static final Color DARK_ORANGE1 = addMapping("darkOrange1",new RGB(255,127,0));
    public static final Color DARK_ORANGE2 = addMapping("darkOrange2",new RGB(238,118,0));
    public static final Color DARK_ORANGE3 = addMapping("darkOrange3",new RGB(205,102,0));
    public static final Color DARK_ORANGE4 = addMapping("darkOrange4",new RGB(139,69,0));
    public static final Color DARK_ORCHID = addMapping("darkOrchid",new RGB(153,50,204));
    public static final Color DARK_ORCHID1 = addMapping("darkOrchid1",new RGB(191,62,255));
    public static final Color DARK_ORCHID2 = addMapping("darkOrchid2",new RGB(178,58,238));
    public static final Color DARK_ORCHID3 = addMapping("darkOrchid3",new RGB(154,50,205));
    public static final Color DARK_ORCHID4 = addMapping("darkOrchid4",new RGB(104,34,139));
    public static final Color DARK_RED = addMapping("darkRed",new RGB(139,0,0));
    public static final Color DARK_SALMON = addMapping("darkSalmon",new RGB(233,150,122));
    public static final Color DARK_SEA_GREEN = addMapping("darkSeaGreen",new RGB(143,188,143));
    public static final Color DARK_SEA_GREEN1 = addMapping("darkSeaGreen1",new RGB(193,255,193));
    public static final Color DARK_SEA_GREEN2 = addMapping("darkSeaGreen2",new RGB(180,238,180));
    public static final Color DARK_SEA_GREEN3 = addMapping("darkSeaGreen3",new RGB(155,205,155));
    public static final Color DARK_SEA_GREEN4 = addMapping("darkSeaGreen4",new RGB(105,139,105));
    public static final Color DARK_SLATE_BLUE = addMapping("darkSlateBlue",new RGB(72,61,139));
    public static final Color DARK_SLATE_GREY = addMapping("darkSlateGrey",new RGB(47,79,79));
    public static final Color DARK_TURQUOISE = addMapping("darkTurquoise",new RGB(0,206,209));
    public static final Color DARK_VIOLET = addMapping("darkViolet",new RGB(148,0,211));
    public static final Color DEEP_PINK = addMapping("deepPink",new RGB(255,20,147));
    public static final Color DEEP_PINK1 = addMapping("deepPink1",new RGB(255,20,147));
    public static final Color DEEP_PINK2 = addMapping("deepPink2",new RGB(238,18,137));
    public static final Color DEEP_PINK3 = addMapping("deepPink3",new RGB(205,16,118));
    public static final Color DEEP_PINK4 = addMapping("deepPink4",new RGB(139,10,80));
    public static final Color DEEP_SKY_BLUE = addMapping("deepSkyBlue",new RGB(0,191,255));
    public static final Color DEEP_SKY_BLUE1 = addMapping("deepSkyBlue1",new RGB(0,191,255));
    public static final Color DEEP_SKY_BLUE2 = addMapping("deepSkyBlue2",new RGB(0,178,238));
    public static final Color DEEP_SKY_BLUE3 = addMapping("deepSkyBlue3",new RGB(0,154,205));
    public static final Color DEEP_SKY_BLUE4 = addMapping("deepSkyBlue4",new RGB(0,104,139));
    public static final Color DIM_GREY = addMapping("dimGrey",new RGB(105,105,105));
    public static final Color DODGER_BLUE = addMapping("dodgerBlue",new RGB(30,144,255));
    public static final Color DODGER_BLUE1 = addMapping("dodgerBlue1",new RGB(30,144,255));
    public static final Color DODGER_BLUE2 = addMapping("dodgerBlue2",new RGB(28,134,238));
    public static final Color DODGER_BLUE3 = addMapping("dodgerBlue3",new RGB(24,116,205));
    public static final Color DODGER_BLUE4 = addMapping("dodgerBlue4",new RGB(16,78,139));
    public static final Color FLORAL_WHITE = addMapping("floralWhite",new RGB(255,250,240));
    public static final Color FOREST_GREEN = addMapping("forestGreen",new RGB(34,139,34));
    public static final Color GHOST_WHITE = addMapping("ghostWhite",new RGB(248,248,255));
    public static final Color GREEN_YELLOW = addMapping("greenYellow",new RGB(173,255,47));
    public static final Color HOT_PINK = addMapping("hotPink",new RGB(255,105,180));
    public static final Color HOT_PINK1 = addMapping("hotPink1",new RGB(255,110,180));
    public static final Color HOT_PINK2 = addMapping("hotPink2",new RGB(238,106,167));
    public static final Color HOT_PINK3 = addMapping("hotPink3",new RGB(205,96,144));
    public static final Color HOT_PINK4 = addMapping("hotPink4",new RGB(139,58,98));
    public static final Color INDIAN_RED = addMapping("indianRed",new RGB(205,92,92));
    public static final Color INDIAN_RED1 = addMapping("indianRed1",new RGB(255,106,106));
    public static final Color INDIAN_RED2 = addMapping("indianRed2",new RGB(238,99,99));
    public static final Color INDIAN_RED3 = addMapping("indianRed3",new RGB(205,85,85));
    public static final Color INDIAN_RED4 = addMapping("indianRed4",new RGB(139,58,58));
    public static final Color LAVENDER_BLUSH = addMapping("lavenderBlush",new RGB(255,240,245));
    public static final Color LAVENDER_BLUSH1 = addMapping("lavenderBlush1",new RGB(255,240,245));
    public static final Color LAVENDER_BLUSH2 = addMapping("lavenderBlush2",new RGB(238,224,229));
    public static final Color LAVENDER_BLUSH3 = addMapping("lavenderBlush3",new RGB(205,193,197));
    public static final Color LAVENDER_BLUSH4 = addMapping("lavenderBlush4",new RGB(139,131,134));
    public static final Color LAWN_GREEN = addMapping("lawnGreen",new RGB(124,252,0));
    public static final Color LEMON_CHIFFON = addMapping("lemonChiffon",new RGB(255,250,205));
    public static final Color LEMON_CHIFFON1 = addMapping("lemonChiffon1",new RGB(255,250,205));
    public static final Color LEMON_CHIFFON2 = addMapping("lemonChiffon2",new RGB(238,233,191));
    public static final Color LEMON_CHIFFON3 = addMapping("lemonChiffon3",new RGB(205,201,165));
    public static final Color LEMON_CHIFFON4 = addMapping("lemonChiffon4",new RGB(139,137,112));
    public static final Color LIGHT_BLUE = addMapping("lightBlue",new RGB(173,216,230));
    public static final Color LIGHT_BLUE1 = addMapping("lightBlue1",new RGB(191,239,255));
    public static final Color LIGHT_BLUE2 = addMapping("lightBlue2",new RGB(178,223,238));
    public static final Color LIGHT_BLUE3 = addMapping("lightBlue3",new RGB(154,192,205));
    public static final Color LIGHT_BLUE4 = addMapping("lightBlue4",new RGB(104,131,139));
    public static final Color LIGHT_CORAL = addMapping("lightCoral",new RGB(240,128,128));
    public static final Color LIGHT_CYAN = addMapping("lightCyan",new RGB(224,255,255));
    public static final Color LIGHT_CYAN1 = addMapping("lightCyan1",new RGB(224,255,255));
    public static final Color LIGHT_CYAN2 = addMapping("lightCyan2",new RGB(209,238,238));
    public static final Color LIGHT_CYAN3 = addMapping("lightCyan3",new RGB(180,205,205));
    public static final Color LIGHT_CYAN4 = addMapping("lightCyan4",new RGB(122,139,139));
    public static final Color LIGHT_GOLDENROD = addMapping("lightGoldenrod",new RGB(238,221,130));
    public static final Color LIGHT_GOLDENROD1 = addMapping("lightGoldenrod1",new RGB(255,236,139));
    public static final Color LIGHT_GOLDENROD2 = addMapping("lightGoldenrod2",new RGB(238,220,130));
    public static final Color LIGHT_GOLDENROD3 = addMapping("lightGoldenrod3",new RGB(205,190,112));
    public static final Color LIGHT_GOLDENROD4 = addMapping("lightGoldenrod4",new RGB(139,129,76));
    public static final Color LIGHT_GOLDENROD_YELLOW = addMapping("lightGoldenrodYellow",new RGB(250,250,210));
    public static final Color LIGHT_GREEN = addMapping("lightGreen",new RGB(144,238,144));
    public static final Color LIGHT_GREY = addMapping("lightGrey",new RGB(211,211,211));
    public static final Color LIGHT_PINK = addMapping("lightPink",new RGB(255,182,193));
    public static final Color LIGHT_PINK1 = addMapping("lightPink1",new RGB(255,174,185));
    public static final Color LIGHT_PINK2 = addMapping("lightPink2",new RGB(238,162,173));
    public static final Color LIGHT_PINK3 = addMapping("lightPink3",new RGB(205,140,149));
    public static final Color LIGHT_PINK4 = addMapping("lightPink4",new RGB(139,95,101));
    public static final Color LIGHT_SALMON = addMapping("lightSalmon",new RGB(255,160,122));
    public static final Color LIGHT_SALMON1 = addMapping("lightSalmon1",new RGB(255,160,122));
    public static final Color LIGHT_SALMON2 = addMapping("lightSalmon2",new RGB(238,149,114));
    public static final Color LIGHT_SALMON3 = addMapping("lightSalmon3",new RGB(205,129,98));
    public static final Color LIGHT_SALMON4 = addMapping("lightSalmon4",new RGB(139,87,66));
    public static final Color LIGHT_SEA_GREEN = addMapping("lightSeaGreen",new RGB(32,178,170));
    public static final Color LIGHT_SKY_BLUE = addMapping("lightSkyBlue",new RGB(135,206,250));
    public static final Color LIGHT_SKY_BLUE1 = addMapping("lightSkyBlue1",new RGB(176,226,255));
    public static final Color LIGHT_SKY_BLUE2 = addMapping("lightSkyBlue2",new RGB(164,211,238));
    public static final Color LIGHT_SKY_BLUE3 = addMapping("lightSkyBlue3",new RGB(141,182,205));
    public static final Color LIGHT_SKY_BLUE4 = addMapping("lightSkyBlue4",new RGB(96,123,139));
    public static final Color LIGHT_SLATE_BLUE = addMapping("lightSlateBlue",new RGB(132,112,255));
    public static final Color LIGHT_SLATE_GREY = addMapping("lightSlateGrey",new RGB(119,136,153));
    public static final Color LIGHT_STEEL_BLUE = addMapping("lightSteelBlue",new RGB(176,196,222));
    public static final Color LIGHT_STEEL_BLUE1 = addMapping("lightSteelBlue1",new RGB(202,225,255));
    public static final Color LIGHT_STEEL_BLUE2 = addMapping("lightSteelBlue2",new RGB(188,210,238));
    public static final Color LIGHT_STEEL_BLUE3 = addMapping("lightSteelBlue3",new RGB(162,181,205));
    public static final Color LIGHT_STEEL_BLUE4 = addMapping("lightSteelBlue4",new RGB(110,123,139));
    public static final Color LIGHT_YELLOW = addMapping("lightYellow",new RGB(255,255,224));
    public static final Color LIGHT_YELLOW1 = addMapping("lightYellow1",new RGB(255,255,224));
    public static final Color LIGHT_YELLOW2 = addMapping("lightYellow2",new RGB(238,238,209));
    public static final Color LIGHT_YELLOW3 = addMapping("lightYellow3",new RGB(205,205,180));
    public static final Color LIGHT_YELLOW4 = addMapping("lightYellow4",new RGB(139,139,122));
    public static final Color LIME_GREEN = addMapping("limeGreen",new RGB(50,205,50));
    public static final Color MEDIUM_AQUAMARINE = addMapping("mediumAquamarine",new RGB(102,205,170));
    public static final Color MEDIUM_BLUE = addMapping("mediumBlue",new RGB(0,0,205));
    public static final Color MEDIUM_ORCHID = addMapping("mediumOrchid",new RGB(186,85,211));
    public static final Color MEDIUM_ORCHID1 = addMapping("mediumOrchid1",new RGB(224,102,255));
    public static final Color MEDIUM_ORCHID2 = addMapping("mediumOrchid2",new RGB(209,95,238));
    public static final Color MEDIUM_ORCHID3 = addMapping("mediumOrchid3",new RGB(180,82,205));
    public static final Color MEDIUM_ORCHID4 = addMapping("mediumOrchid4",new RGB(122,55,139));
    public static final Color MEDIUM_PURPLE = addMapping("mediumPurple",new RGB(147,112,219));
    public static final Color MEDIUM_PURPLE1 = addMapping("mediumPurple1",new RGB(171,130,255));
    public static final Color MEDIUM_PURPLE2 = addMapping("mediumPurple2",new RGB(159,121,238));
    public static final Color MEDIUM_PURPLE3 = addMapping("mediumPurple3",new RGB(137,104,205));
    public static final Color MEDIUM_PURPLE4 = addMapping("mediumPurple4",new RGB(93,71,139));
    public static final Color MEDIUM_SEA_GREEN = addMapping("mediumSeaGreen",new RGB(60,179,113));
    public static final Color MEDIUM_SLATE_BLUE = addMapping("mediumSlateBlue",new RGB(123,104,238));
    public static final Color MEDIUM_SPRING_GREEN = addMapping("mediumSpringGreen",new RGB(0,250,154));
    public static final Color MEDIUM_TURQUOISE = addMapping("mediumTurquoise",new RGB(72,209,204));
    public static final Color MEDIUM_VIOLET_RED = addMapping("mediumVioletRed",new RGB(199,21,133));
    public static final Color MIDNIGHT_BLUE = addMapping("midnightBlue",new RGB(25,25,112));
    public static final Color MINT_CREAM = addMapping("mintCream",new RGB(245,255,250));
    public static final Color MISTY_ROSE = addMapping("mistyRose",new RGB(255,228,225));
    public static final Color MISTY_ROSE1 = addMapping("mistyRose1",new RGB(255,228,225));
    public static final Color MISTY_ROSE2 = addMapping("mistyRose2",new RGB(238,213,210));
    public static final Color MISTY_ROSE3 = addMapping("mistyRose3",new RGB(205,183,181));
    public static final Color MISTY_ROSE4 = addMapping("mistyRose4",new RGB(139,125,123));
    public static final Color NAVAJO_WHITE = addMapping("navajoWhite",new RGB(255,222,173));
    public static final Color NAVAJO_WHITE1 = addMapping("navajoWhite1",new RGB(255,222,173));
    public static final Color NAVAJO_WHITE2 = addMapping("navajoWhite2",new RGB(238,207,161));
    public static final Color NAVAJO_WHITE3 = addMapping("navajoWhite3",new RGB(205,179,139));
    public static final Color NAVAJO_WHITE4 = addMapping("navajoWhite4",new RGB(139,121,94));
    public static final Color NAVY_BLUE = addMapping("navyBlue",new RGB(0,0,128));
    public static final Color OLD_LACE = addMapping("oldLace",new RGB(253,245,230));
    public static final Color OLIVE_DRAB = addMapping("oliveDrab",new RGB(107,142,35));
    public static final Color OLIVE_DRAB1 = addMapping("oliveDrab1",new RGB(192,255,62));
    public static final Color OLIVE_DRAB2 = addMapping("oliveDrab2",new RGB(179,238,58));
    public static final Color OLIVE_DRAB3 = addMapping("oliveDrab3",new RGB(154,205,50));
    public static final Color OLIVE_DRAB4 = addMapping("oliveDrab4",new RGB(105,139,34));
    public static final Color ORANGE_RED = addMapping("orangeRed",new RGB(255,69,0));
    public static final Color ORANGE_RED1 = addMapping("orangeRed1",new RGB(255,69,0));
    public static final Color ORANGE_RED2 = addMapping("orangeRed2",new RGB(238,64,0));
    public static final Color ORANGE_RED3 = addMapping("orangeRed3",new RGB(205,55,0));
    public static final Color ORANGE_RED4 = addMapping("orangeRed4",new RGB(139,37,0));
    public static final Color PALE_GOLDENROD = addMapping("paleGoldenrod",new RGB(238,232,170));
    public static final Color PALE_GREEN = addMapping("paleGreen",new RGB(152,251,152));
    public static final Color PALE_GREEN1 = addMapping("paleGreen1",new RGB(154,255,154));
    public static final Color PALE_GREEN2 = addMapping("paleGreen2",new RGB(144,238,144));
    public static final Color PALE_GREEN3 = addMapping("paleGreen3",new RGB(124,205,124));
    public static final Color PALE_GREEN4 = addMapping("paleGreen4",new RGB(84,139,84));
    public static final Color PALE_TURQUOISE = addMapping("paleTurquoise",new RGB(175,238,238));
    public static final Color PALE_TURQUOISE1 = addMapping("paleTurquoise1",new RGB(187,255,255));
    public static final Color PALE_TURQUOISE2 = addMapping("paleTurquoise2",new RGB(174,238,238));
    public static final Color PALE_TURQUOISE3 = addMapping("paleTurquoise3",new RGB(150,205,205));
    public static final Color PALE_TURQUOISE4 = addMapping("paleTurquoise4",new RGB(102,139,139));
    public static final Color PALE_VIOLET_RED = addMapping("paleVioletRed",new RGB(219,112,147));
    public static final Color PALE_VIOLET_RED1 = addMapping("paleVioletRed1",new RGB(255,130,171));
    public static final Color PALE_VIOLET_RED2 = addMapping("paleVioletRed2",new RGB(238,121,159));
    public static final Color PALE_VIOLET_RED3 = addMapping("paleVioletRed3",new RGB(205,104,137));
    public static final Color PALE_VIOLET_RED4 = addMapping("paleVioletRed4",new RGB(139,71,93));
    public static final Color PAPAYA_WHIP = addMapping("papayaWhip",new RGB(255,239,213));
    public static final Color PEACH_PUFF = addMapping("peachPuff",new RGB(255,218,185));
    public static final Color PEACH_PUFF1 = addMapping("peachPuff1",new RGB(255,218,185));
    public static final Color PEACH_PUFF2 = addMapping("peachPuff2",new RGB(238,203,173));
    public static final Color PEACH_PUFF3 = addMapping("peachPuff3",new RGB(205,175,149));
    public static final Color PEACH_PUFF4 = addMapping("peachPuff4",new RGB(139,119,101));
    public static final Color POWDER_BLUE = addMapping("powderBlue",new RGB(176,224,230));
    public static final Color ROSY_BROWN = addMapping("rosyBrown",new RGB(188,143,143));
    public static final Color ROSY_BROWN1 = addMapping("rosyBrown1",new RGB(255,193,193));
    public static final Color ROSY_BROWN2 = addMapping("rosyBrown2",new RGB(238,180,180));
    public static final Color ROSY_BROWN3 = addMapping("rosyBrown3",new RGB(205,155,155));
    public static final Color ROSY_BROWN4 = addMapping("rosyBrown4",new RGB(139,105,105));
    public static final Color ROYAL_BLUE = addMapping("royalBlue",new RGB(65,105,225));
    public static final Color ROYAL_BLUE1 = addMapping("royalBlue1",new RGB(72,118,255));
    public static final Color ROYAL_BLUE2 = addMapping("royalBlue2",new RGB(67,110,238));
    public static final Color ROYAL_BLUE3 = addMapping("royalBlue3",new RGB(58,95,205));
    public static final Color ROYAL_BLUE4 = addMapping("royalBlue4",new RGB(39,64,139));
    public static final Color SADDLE_BROWN = addMapping("saddleBrown",new RGB(139,69,19));
    public static final Color SANDY_BROWN = addMapping("sandyBrown",new RGB(244,164,96));
    public static final Color SEA_GREEN = addMapping("seaGreen",new RGB(46,139,87));
    public static final Color SEA_GREEN1 = addMapping("seaGreen1",new RGB(84,255,159));
    public static final Color SEA_GREEN2 = addMapping("seaGreen2",new RGB(78,238,148));
    public static final Color SEA_GREEN3 = addMapping("seaGreen3",new RGB(67,205,128));
    public static final Color SEA_GREEN4 = addMapping("seaGreen4",new RGB(46,139,87));
    public static final Color SKY_BLUE = addMapping("skyBlue",new RGB(135,206,235));
    public static final Color SKY_BLUE1 = addMapping("skyBlue1",new RGB(135,206,255));
    public static final Color SKY_BLUE2 = addMapping("skyBlue2",new RGB(126,192,238));
    public static final Color SKY_BLUE3 = addMapping("skyBlue3",new RGB(108,166,205));
    public static final Color SKY_BLUE4 = addMapping("skyBlue4",new RGB(74,112,139));
    public static final Color SLATE_BLUE = addMapping("slateBlue",new RGB(106,90,205));
    public static final Color SLATE_BLUE1 = addMapping("slateBlue1",new RGB(131,111,255));
    public static final Color SLATE_BLUE2 = addMapping("slateBlue2",new RGB(122,103,238));
    public static final Color SLATE_BLUE3 = addMapping("slateBlue3",new RGB(105,89,205));
    public static final Color SLATE_BLUE4 = addMapping("slateBlue4",new RGB(71,60,139));
    public static final Color SLATE_GREY = addMapping("slateGrey",new RGB(112,128,144));
    public static final Color SPRING_GREEN = addMapping("springGreen",new RGB(0,255,127));
    public static final Color SPRING_GREEN1 = addMapping("springGreen1",new RGB(0,255,127));
    public static final Color SPRING_GREEN2 = addMapping("springGreen2",new RGB(0,238,118));
    public static final Color SPRING_GREEN3 = addMapping("springGreen3",new RGB(0,205,102));
    public static final Color SPRING_GREEN4 = addMapping("springGreen4",new RGB(0,139,69));
    public static final Color STEEL_BLUE = addMapping("steelBlue",new RGB(70,130,180));
    public static final Color STEEL_BLUE1 = addMapping("steelBlue1",new RGB(99,184,255));
    public static final Color STEEL_BLUE2 = addMapping("steelBlue2",new RGB(92,172,238));
    public static final Color STEEL_BLUE3 = addMapping("steelBlue3",new RGB(79,148,205));
    public static final Color STEEL_BLUE4 = addMapping("steelBlue4",new RGB(54,100,139));
    public static final Color VIOLET_RED = addMapping("violetRed",new RGB(208,32,144));
    public static final Color VIOLET_RED1 = addMapping("violetRed1",new RGB(255,62,150));
    public static final Color VIOLET_RED2 = addMapping("violetRed2",new RGB(238,58,140));
    public static final Color VIOLET_RED3 = addMapping("violetRed3",new RGB(205,50,120));
    public static final Color VIOLET_RED4 = addMapping("violetRed4",new RGB(139,34,82));
    public static final Color WHITE_SMOKE = addMapping("whiteSmoke",new RGB(245,245,245));
    public static final Color YELLOW_GREEN = addMapping("yellowGreen",new RGB(154,205,50));
    public static final Color AQUAMARINE = addMapping("aquamarine",new RGB(127,255,212));
    public static final Color AQUAMARINE1 = addMapping("aquamarine1",new RGB(127,255,212));
    public static final Color AQUAMARINE2 = addMapping("aquamarine2",new RGB(118,238,198));
    public static final Color AQUAMARINE3 = addMapping("aquamarine3",new RGB(102,205,170));
    public static final Color AQUAMARINE4 = addMapping("aquamarine4",new RGB(69,139,116));
    public static final Color AZURE = addMapping("azure",new RGB(240,255,255));
    public static final Color AZURE1 = addMapping("azure1",new RGB(240,255,255));
    public static final Color AZURE2 = addMapping("azure2",new RGB(224,238,238));
    public static final Color AZURE3 = addMapping("azure3",new RGB(193,205,205));
    public static final Color AZURE4 = addMapping("azure4",new RGB(131,139,139));
    public static final Color BEIGE = addMapping("beige",new RGB(245,245,220));
    public static final Color BISQUE = addMapping("bisque",new RGB(255,228,196));
    public static final Color BISQUE1 = addMapping("bisque1",new RGB(255,228,196));
    public static final Color BISQUE2 = addMapping("bisque2",new RGB(238,213,183));
    public static final Color BISQUE3 = addMapping("bisque3",new RGB(205,183,158));
    public static final Color BISQUE4 = addMapping("bisque4",new RGB(139,125,107));
    public static final Color BLACK = addMapping("black",new RGB(0,0,0));
    public static final Color BLUE = addMapping("blue",new RGB(0,0,255));
    public static final Color BLUE1 = addMapping("blue1",new RGB(0,0,255));
    public static final Color BLUE2 = addMapping("blue2",new RGB(0,0,238));
    public static final Color BLUE3 = addMapping("blue3",new RGB(0,0,205));
    public static final Color BLUE4 = addMapping("blue4",new RGB(0,0,139));
    public static final Color BROWN = addMapping("brown",new RGB(165,42,42));
    public static final Color BROWN1 = addMapping("brown1",new RGB(255,64,64));
    public static final Color BROWN2 = addMapping("brown2",new RGB(238,59,59));
    public static final Color BROWN3 = addMapping("brown3",new RGB(205,51,51));
    public static final Color BROWN4 = addMapping("brown4",new RGB(139,35,35));
    public static final Color BURLYWOOD = addMapping("burlywood",new RGB(222,184,135));
    public static final Color BURLYWOOD1 = addMapping("burlywood1",new RGB(255,211,155));
    public static final Color BURLYWOOD2 = addMapping("burlywood2",new RGB(238,197,145));
    public static final Color BURLYWOOD3 = addMapping("burlywood3",new RGB(205,170,125));
    public static final Color BURLYWOOD4 = addMapping("burlywood4",new RGB(139,115,85));
    public static final Color CHARTREUSE = addMapping("chartreuse",new RGB(127,255,0));
    public static final Color CHARTREUSE1 = addMapping("chartreuse1",new RGB(127,255,0));
    public static final Color CHARTREUSE2 = addMapping("chartreuse2",new RGB(118,238,0));
    public static final Color CHARTREUSE3 = addMapping("chartreuse3",new RGB(102,205,0));
    public static final Color CHARTREUSE4 = addMapping("chartreuse4",new RGB(69,139,0));
    public static final Color CHOCOLATE = addMapping("chocolate",new RGB(210,105,30));
    public static final Color CHOCOLATE1 = addMapping("chocolate1",new RGB(255,127,36));
    public static final Color CHOCOLATE2 = addMapping("chocolate2",new RGB(238,118,33));
    public static final Color CHOCOLATE3 = addMapping("chocolate3",new RGB(205,102,29));
    public static final Color CHOCOLATE4 = addMapping("chocolate4",new RGB(139,69,19));
    public static final Color CORAL = addMapping("coral",new RGB(255,127,80));
    public static final Color CORAL1 = addMapping("coral1",new RGB(255,114,86));
    public static final Color CORAL2 = addMapping("coral2",new RGB(238,106,80));
    public static final Color CORAL3 = addMapping("coral3",new RGB(205,91,69));
    public static final Color CORAL4 = addMapping("coral4",new RGB(139,62,47));
    public static final Color CORNSILK = addMapping("cornsilk",new RGB(255,248,220));
    public static final Color CORNSILK1 = addMapping("cornsilk1",new RGB(255,248,220));
    public static final Color CORNSILK2 = addMapping("cornsilk2",new RGB(238,232,205));
    public static final Color CORNSILK3 = addMapping("cornsilk3",new RGB(205,200,177));
    public static final Color CORNSILK4 = addMapping("cornsilk4",new RGB(139,136,120));
    public static final Color CYAN = addMapping("cyan",new RGB(0,255,255));
    public static final Color CYAN1 = addMapping("cyan1",new RGB(0,255,255));
    public static final Color CYAN2 = addMapping("cyan2",new RGB(0,238,238));
    public static final Color CYAN3 = addMapping("cyan3",new RGB(0,205,205));
    public static final Color CYAN4 = addMapping("cyan4",new RGB(0,139,139));
    public static final Color FIREBRICK = addMapping("firebrick",new RGB(178,34,34));
    public static final Color FIREBRICK1 = addMapping("firebrick1",new RGB(255,48,48));
    public static final Color FIREBRICK2 = addMapping("firebrick2",new RGB(238,44,44));
    public static final Color FIREBRICK3 = addMapping("firebrick3",new RGB(205,38,38));
    public static final Color FIREBRICK4 = addMapping("firebrick4",new RGB(139,26,26));
    public static final Color GAINSBORO = addMapping("gainsboro",new RGB(220,220,220));
    public static final Color GOLD = addMapping("gold",new RGB(255,215,0));
    public static final Color GOLD1 = addMapping("gold1",new RGB(255,215,0));
    public static final Color GOLD2 = addMapping("gold2",new RGB(238,201,0));
    public static final Color GOLD3 = addMapping("gold3",new RGB(205,173,0));
    public static final Color GOLD4 = addMapping("gold4",new RGB(139,117,0));
    public static final Color GOLDENROD = addMapping("goldenrod",new RGB(218,165,32));
    public static final Color GOLDENROD1 = addMapping("goldenrod1",new RGB(255,193,37));
    public static final Color GOLDENROD2 = addMapping("goldenrod2",new RGB(238,180,34));
    public static final Color GOLDENROD3 = addMapping("goldenrod3",new RGB(205,155,29));
    public static final Color GOLDENROD4 = addMapping("goldenrod4",new RGB(139,105,20));
    public static final Color GREEN = addMapping("green",new RGB(0,255,0));
    public static final Color GREEN1 = addMapping("green1",new RGB(0,255,0));
    public static final Color GREEN2 = addMapping("green2",new RGB(0,238,0));
    public static final Color GREEN3 = addMapping("green3",new RGB(0,205,0));
    public static final Color GREEN4 = addMapping("green4",new RGB(0,139,0));
    public static final Color GREY = addMapping("grey",new RGB(192,192,192));
    public static final Color GREY0 = addMapping("grey0",new RGB(0,0,0));
    public static final Color GREY1 = addMapping("grey1",new RGB(3,3,3));
    public static final Color GREY10 = addMapping("grey10",new RGB(26,26,26));
    public static final Color GREY100 = addMapping("grey100",new RGB(255,255,255));
    public static final Color GREY11 = addMapping("grey11",new RGB(28,28,28));
    public static final Color GREY12 = addMapping("grey12",new RGB(31,31,31));
    public static final Color GREY13 = addMapping("grey13",new RGB(33,33,33));
    public static final Color GREY14 = addMapping("grey14",new RGB(36,36,36));
    public static final Color GREY15 = addMapping("grey15",new RGB(38,38,38));
    public static final Color GREY16 = addMapping("grey16",new RGB(41,41,41));
    public static final Color GREY17 = addMapping("grey17",new RGB(43,43,43));
    public static final Color GREY18 = addMapping("grey18",new RGB(46,46,46));
    public static final Color GREY19 = addMapping("grey19",new RGB(48,48,48));
    public static final Color GREY2 = addMapping("grey2",new RGB(5,5,5));
    public static final Color GREY20 = addMapping("grey20",new RGB(51,51,51));
    public static final Color GREY21 = addMapping("grey21",new RGB(54,54,54));
    public static final Color GREY22 = addMapping("grey22",new RGB(56,56,56));
    public static final Color GREY23 = addMapping("grey23",new RGB(59,59,59));
    public static final Color GREY24 = addMapping("grey24",new RGB(61,61,61));
    public static final Color GREY25 = addMapping("grey25",new RGB(64,64,64));
    public static final Color GREY26 = addMapping("grey26",new RGB(66,66,66));
    public static final Color GREY27 = addMapping("grey27",new RGB(69,69,69));
    public static final Color GREY28 = addMapping("grey28",new RGB(71,71,71));
    public static final Color GREY29 = addMapping("grey29",new RGB(74,74,74));
    public static final Color GREY3 = addMapping("grey3",new RGB(8,8,8));
    public static final Color GREY30 = addMapping("grey30",new RGB(77,77,77));
    public static final Color GREY31 = addMapping("grey31",new RGB(79,79,79));
    public static final Color GREY32 = addMapping("grey32",new RGB(82,82,82));
    public static final Color GREY33 = addMapping("grey33",new RGB(84,84,84));
    public static final Color GREY34 = addMapping("grey34",new RGB(87,87,87));
    public static final Color GREY35 = addMapping("grey35",new RGB(89,89,89));
    public static final Color GREY36 = addMapping("grey36",new RGB(92,92,92));
    public static final Color GREY37 = addMapping("grey37",new RGB(94,94,94));
    public static final Color GREY38 = addMapping("grey38",new RGB(97,97,97));
    public static final Color GREY39 = addMapping("grey39",new RGB(99,99,99));
    public static final Color GREY4 = addMapping("grey4",new RGB(10,10,10));
    public static final Color GREY40 = addMapping("grey40",new RGB(102,102,102));
    public static final Color GREY41 = addMapping("grey41",new RGB(105,105,105));
    public static final Color GREY42 = addMapping("grey42",new RGB(107,107,107));
    public static final Color GREY43 = addMapping("grey43",new RGB(110,110,110));
    public static final Color GREY44 = addMapping("grey44",new RGB(112,112,112));
    public static final Color GREY45 = addMapping("grey45",new RGB(115,115,115));
    public static final Color GREY46 = addMapping("grey46",new RGB(117,117,117));
    public static final Color GREY47 = addMapping("grey47",new RGB(120,120,120));
    public static final Color GREY48 = addMapping("grey48",new RGB(122,122,122));
    public static final Color GREY49 = addMapping("grey49",new RGB(125,125,125));
    public static final Color GREY5 = addMapping("grey5",new RGB(13,13,13));
    public static final Color GREY50 = addMapping("grey50",new RGB(127,127,127));
    public static final Color GREY51 = addMapping("grey51",new RGB(130,130,130));
    public static final Color GREY52 = addMapping("grey52",new RGB(133,133,133));
    public static final Color GREY53 = addMapping("grey53",new RGB(135,135,135));
    public static final Color GREY54 = addMapping("grey54",new RGB(138,138,138));
    public static final Color GREY55 = addMapping("grey55",new RGB(140,140,140));
    public static final Color GREY56 = addMapping("grey56",new RGB(143,143,143));
    public static final Color GREY57 = addMapping("grey57",new RGB(145,145,145));
    public static final Color GREY58 = addMapping("grey58",new RGB(148,148,148));
    public static final Color GREY59 = addMapping("grey59",new RGB(150,150,150));
    public static final Color GREY6 = addMapping("grey6",new RGB(15,15,15));
    public static final Color GREY60 = addMapping("grey60",new RGB(153,153,153));
    public static final Color GREY61 = addMapping("grey61",new RGB(156,156,156));
    public static final Color GREY62 = addMapping("grey62",new RGB(158,158,158));
    public static final Color GREY63 = addMapping("grey63",new RGB(161,161,161));
    public static final Color GREY64 = addMapping("grey64",new RGB(163,163,163));
    public static final Color GREY65 = addMapping("grey65",new RGB(166,166,166));
    public static final Color GREY66 = addMapping("grey66",new RGB(168,168,168));
    public static final Color GREY67 = addMapping("grey67",new RGB(171,171,171));
    public static final Color GREY68 = addMapping("grey68",new RGB(173,173,173));
    public static final Color GREY69 = addMapping("grey69",new RGB(176,176,176));
    public static final Color GREY7 = addMapping("grey7",new RGB(18,18,18));
    public static final Color GREY70 = addMapping("grey70",new RGB(179,179,179));
    public static final Color GREY71 = addMapping("grey71",new RGB(181,181,181));
    public static final Color GREY72 = addMapping("grey72",new RGB(184,184,184));
    public static final Color GREY73 = addMapping("grey73",new RGB(186,186,186));
    public static final Color GREY74 = addMapping("grey74",new RGB(189,189,189));
    public static final Color GREY75 = addMapping("grey75",new RGB(191,191,191));
    public static final Color GREY76 = addMapping("grey76",new RGB(194,194,194));
    public static final Color GREY77 = addMapping("grey77",new RGB(196,196,196));
    public static final Color GREY78 = addMapping("grey78",new RGB(199,199,199));
    public static final Color GREY79 = addMapping("grey79",new RGB(201,201,201));
    public static final Color GREY8 = addMapping("grey8",new RGB(20,20,20));
    public static final Color GREY80 = addMapping("grey80",new RGB(204,204,204));
    public static final Color GREY81 = addMapping("grey81",new RGB(207,207,207));
    public static final Color GREY82 = addMapping("grey82",new RGB(209,209,209));
    public static final Color GREY83 = addMapping("grey83",new RGB(212,212,212));
    public static final Color GREY84 = addMapping("grey84",new RGB(214,214,214));
    public static final Color GREY85 = addMapping("grey85",new RGB(217,217,217));
    public static final Color GREY86 = addMapping("grey86",new RGB(219,219,219));
    public static final Color GREY87 = addMapping("grey87",new RGB(222,222,222));
    public static final Color GREY88 = addMapping("grey88",new RGB(224,224,224));
    public static final Color GREY89 = addMapping("grey89",new RGB(227,227,227));
    public static final Color GREY9 = addMapping("grey9",new RGB(23,23,23));
    public static final Color GREY90 = addMapping("grey90",new RGB(229,229,229));
    public static final Color GREY91 = addMapping("grey91",new RGB(232,232,232));
    public static final Color GREY92 = addMapping("grey92",new RGB(235,235,235));
    public static final Color GREY93 = addMapping("grey93",new RGB(237,237,237));
    public static final Color GREY94 = addMapping("grey94",new RGB(240,240,240));
    public static final Color GREY95 = addMapping("grey95",new RGB(242,242,242));
    public static final Color GREY96 = addMapping("grey96",new RGB(245,245,245));
    public static final Color GREY97 = addMapping("grey97",new RGB(247,247,247));
    public static final Color GREY98 = addMapping("grey98",new RGB(250,250,250));
    public static final Color GREY99 = addMapping("grey99",new RGB(252,252,252));
    public static final Color HONEYDEW = addMapping("honeydew",new RGB(240,255,240));
    public static final Color HONEYDEW1 = addMapping("honeydew1",new RGB(240,255,240));
    public static final Color HONEYDEW2 = addMapping("honeydew2",new RGB(224,238,224));
    public static final Color HONEYDEW3 = addMapping("honeydew3",new RGB(193,205,193));
    public static final Color HONEYDEW4 = addMapping("honeydew4",new RGB(131,139,131));
    public static final Color IVORY = addMapping("ivory",new RGB(255,255,240));
    public static final Color IVORY1 = addMapping("ivory1",new RGB(255,255,240));
    public static final Color IVORY2 = addMapping("ivory2",new RGB(238,238,224));
    public static final Color IVORY3 = addMapping("ivory3",new RGB(205,205,193));
    public static final Color IVORY4 = addMapping("ivory4",new RGB(139,139,131));
    public static final Color KHAKI = addMapping("khaki",new RGB(240,230,140));
    public static final Color KHAKI1 = addMapping("khaki1",new RGB(255,246,143));
    public static final Color KHAKI2 = addMapping("khaki2",new RGB(238,230,133));
    public static final Color KHAKI3 = addMapping("khaki3",new RGB(205,198,115));
    public static final Color KHAKI4 = addMapping("khaki4",new RGB(139,134,78));
    public static final Color LAVENDER = addMapping("lavender",new RGB(230,230,250));
    public static final Color LINEN = addMapping("linen",new RGB(250,240,230));
    public static final Color MAGENTA = addMapping("magenta",new RGB(255,0,255));
    public static final Color MAGENTA1 = addMapping("magenta1",new RGB(255,0,255));
    public static final Color MAGENTA2 = addMapping("magenta2",new RGB(238,0,238));
    public static final Color MAGENTA3 = addMapping("magenta3",new RGB(205,0,205));
    public static final Color MAGENTA4 = addMapping("magenta4",new RGB(139,0,139));
    public static final Color MAROON = addMapping("maroon",new RGB(176,48,96));
    public static final Color MAROON1 = addMapping("maroon1",new RGB(255,52,179));
    public static final Color MAROON2 = addMapping("maroon2",new RGB(238,48,167));
    public static final Color MAROON3 = addMapping("maroon3",new RGB(205,41,144));
    public static final Color MAROON4 = addMapping("maroon4",new RGB(139,28,98));
    public static final Color MOCCASIN = addMapping("moccasin",new RGB(255,228,181));
    public static final Color NAVY = addMapping("navy",new RGB(0,0,128));
    public static final Color ORANGE = addMapping("orange",new RGB(255,165,0));
    public static final Color ORANGE1 = addMapping("orange1",new RGB(255,165,0));
    public static final Color ORANGE2 = addMapping("orange2",new RGB(238,154,0));
    public static final Color ORANGE3 = addMapping("orange3",new RGB(205,133,0));
    public static final Color ORANGE4 = addMapping("orange4",new RGB(139,90,0));
    public static final Color ORCHID = addMapping("orchid",new RGB(218,112,214));
    public static final Color ORCHID1 = addMapping("orchid1",new RGB(255,131,250));
    public static final Color ORCHID2 = addMapping("orchid2",new RGB(238,122,233));
    public static final Color ORCHID3 = addMapping("orchid3",new RGB(205,105,201));
    public static final Color ORCHID4 = addMapping("orchid4",new RGB(139,71,137));
    public static final Color PERU = addMapping("peru",new RGB(205,133,63));
    public static final Color PINK = addMapping("pink",new RGB(255,192,203));
    public static final Color PINK1 = addMapping("pink1",new RGB(255,181,197));
    public static final Color PINK2 = addMapping("pink2",new RGB(238,169,184));
    public static final Color PINK3 = addMapping("pink3",new RGB(205,145,158));
    public static final Color PINK4 = addMapping("pink4",new RGB(139,99,108));
    public static final Color PLUM = addMapping("plum",new RGB(221,160,221));
    public static final Color PLUM1 = addMapping("plum1",new RGB(255,187,255));
    public static final Color PLUM2 = addMapping("plum2",new RGB(238,174,238));
    public static final Color PLUM3 = addMapping("plum3",new RGB(205,150,205));
    public static final Color PLUM4 = addMapping("plum4",new RGB(139,102,139));
    public static final Color PURPLE = addMapping("purple",new RGB(160,32,240));
    public static final Color PURPLE1 = addMapping("purple1",new RGB(155,48,255));
    public static final Color PURPLE2 = addMapping("purple2",new RGB(145,44,238));
    public static final Color PURPLE3 = addMapping("purple3",new RGB(125,38,205));
    public static final Color PURPLE4 = addMapping("purple4",new RGB(85,26,139));
    public static final Color RED = addMapping("red",new RGB(255,0,0));
    public static final Color RED1 = addMapping("red1",new RGB(255,0,0));
    public static final Color RED2 = addMapping("red2",new RGB(238,0,0));
    public static final Color RED3 = addMapping("red3",new RGB(205,0,0));
    public static final Color RED4 = addMapping("red4",new RGB(139,0,0));
    public static final Color SALMON = addMapping("salmon",new RGB(250,128,114));
    public static final Color SALMON1 = addMapping("salmon1",new RGB(255,140,105));
    public static final Color SALMON2 = addMapping("salmon2",new RGB(238,130,98));
    public static final Color SALMON3 = addMapping("salmon3",new RGB(205,112,84));
    public static final Color SALMON4 = addMapping("salmon4",new RGB(139,76,57));
    public static final Color SEASHELL = addMapping("seashell",new RGB(255,245,238));
    public static final Color SEASHELL1 = addMapping("seashell1",new RGB(255,245,238));
    public static final Color SEASHELL2 = addMapping("seashell2",new RGB(238,229,222));
    public static final Color SEASHELL3 = addMapping("seashell3",new RGB(205,197,191));
    public static final Color SEASHELL4 = addMapping("seashell4",new RGB(139,134,130));
    public static final Color SIENNA = addMapping("sienna",new RGB(160,82,45));
    public static final Color SIENNA1 = addMapping("sienna1",new RGB(255,130,71));
    public static final Color SIENNA2 = addMapping("sienna2",new RGB(238,121,66));
    public static final Color SIENNA3 = addMapping("sienna3",new RGB(205,104,57));
    public static final Color SIENNA4 = addMapping("sienna4",new RGB(139,71,38));
    public static final Color SNOW = addMapping("snow",new RGB(255,250,250));
    public static final Color SNOW1 = addMapping("snow1",new RGB(255,250,250));
    public static final Color SNOW2 = addMapping("snow2",new RGB(238,233,233));
    public static final Color SNOW3 = addMapping("snow3",new RGB(205,201,201));
    public static final Color SNOW4 = addMapping("snow4",new RGB(139,137,137));
    public static final Color TAN = addMapping("tan",new RGB(210,180,140));
    public static final Color TAN1 = addMapping("tan1",new RGB(255,165,79));
    public static final Color TAN2 = addMapping("tan2",new RGB(238,154,73));
    public static final Color TAN3 = addMapping("tan3",new RGB(205,133,63));
    public static final Color TAN4 = addMapping("tan4",new RGB(139,90,43));
    public static final Color THISTLE = addMapping("thistle",new RGB(216,191,216));
    public static final Color THISTLE1 = addMapping("thistle1",new RGB(255,225,255));
    public static final Color THISTLE2 = addMapping("thistle2",new RGB(238,210,238));
    public static final Color THISTLE3 = addMapping("thistle3",new RGB(205,181,205));
    public static final Color THISTLE4 = addMapping("thistle4",new RGB(139,123,139));
    public static final Color TOMATO = addMapping("tomato",new RGB(255,99,71));
    public static final Color TOMATO1 = addMapping("tomato1",new RGB(255,99,71));
    public static final Color TOMATO2 = addMapping("tomato2",new RGB(238,92,66));
    public static final Color TOMATO3 = addMapping("tomato3",new RGB(205,79,57));
    public static final Color TOMATO4 = addMapping("tomato4",new RGB(139,54,38));
    public static final Color TURQUOISE = addMapping("turquoise",new RGB(64,224,208));
    public static final Color TURQUOISE1 = addMapping("turquoise1",new RGB(0,245,255));
    public static final Color TURQUOISE2 = addMapping("turquoise2",new RGB(0,229,238));
    public static final Color TURQUOISE3 = addMapping("turquoise3",new RGB(0,197,205));
    public static final Color TURQUOISE4 = addMapping("turquoise4",new RGB(0,134,139));
    public static final Color VIOLET = addMapping("violet",new RGB(238,130,238));
    public static final Color WHEAT = addMapping("wheat",new RGB(245,222,179));
    public static final Color WHEAT1 = addMapping("wheat1",new RGB(255,231,186));
    public static final Color WHEAT2 = addMapping("wheat2",new RGB(238,216,174));
    public static final Color WHEAT3 = addMapping("wheat3",new RGB(205,186,150));
    public static final Color WHEAT4 = addMapping("wheat4",new RGB(139,126,102));
    public static final Color WHITE = addMapping("white",new RGB(255,255,255));
    public static final Color YELLOW = addMapping("yellow",new RGB(255,255,0));
    public static final Color YELLOW1 = addMapping("yellow1",new RGB(255,255,0));
    public static final Color YELLOW2 = addMapping("yellow2",new RGB(238,238,0));
    public static final Color YELLOW3 = addMapping("yellow3",new RGB(205,205,0));
    public static final Color YELLOW4 = addMapping("yellow4",new RGB(139,139,0));

    static {

        goodDarkColors = new Color[] {
        		ColorFactory.asColor("darkblue"),
        		ColorFactory.asColor("red2"),
        		ColorFactory.asColor("darkGreen"),
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
        		//XXX add more
        };

        goodLightColors = new Color[] {
        		ColorFactory.asColor("lightCyan"),
        		ColorFactory.asColor("lightCoral"),
        		ColorFactory.asColor("lightBlue"),
        		ColorFactory.asColor("lightGreen"),
        		ColorFactory.asColor("lightYellow"),
        		ColorFactory.asColor("plum1"),
        		ColorFactory.asColor("lightSalmon"),
        		ColorFactory.asColor("lightPink"),
        		ColorFactory.asColor("lightGrey"),
        		ColorFactory.asColor("mediumPurple"),
        		//XXX add more
        };
	};

    /**
     * label provider to display color rectangles
     * @author rhornig
     */
    public static class ColorLabelProvider extends LabelProvider {
        @Override
        public Image getImage(Object element) {
            return ColorFactory.asImage((String)element);
        }

    }

    /**
     * Returns the color name, or #RRGGBB if no constant found for the color. Throws
     * <code>NullPointerException</code> if the parameter is <code>null</code>
     * @param value RGB object to be converted
     * @return String representation of the color
     */
    public static String asString(RGB value) {
        // check for color constants first
        String constName = rgb2strMap.get(value);
        if (constName != null) return constName;

        // if no constant was found, create a hex description
        return String.format("#%06X", value.red << 16 | value.green << 8 | value.blue);
    }

    /**
     * Parses a string into an RGB object. The format can be #RRGGBB @HHSSII or
     * a color constant name like: red, green
     * Throws null pointer exception on incoming <code>null</code>
     * @param value String to be parsed
     * @return RGB or <code>null</code> on parse error
     */
    public static RGB asRGB(String value) {
        RGB result = null;
        try {
            if (value == null)
                return null;
            // check for color constants
            RGB constRGB = str2rgbRegistry.getRGB(value.toLowerCase());
            // return a new RGB object, because RGB is mutable
            if (constRGB != null)
                return new RGB(constRGB.red, constRGB.green, constRGB.blue);

            // if no constant name found, try to parse as hex string
            if (value.startsWith("#")) {
                int rgbVal = Integer.parseInt(value.substring(1), 16);
                result = new RGB((rgbVal >> 16) & 0xFF, (rgbVal >> 8) & 0xFF, rgbVal & 0xFF);
            }
            // check for HSB syntax and convert
            if (value.startsWith("@")) {
                int hsbVal = Integer.parseInt(value.substring(1), 16);
                float hue = ((hsbVal >> 16) & 0xFF) / 256.0f * 360.0f;  // 0..360
                float saturation = ((hsbVal >> 8) & 0xFF) / 256.0f ;    // 0..1
                float brightness = (hsbVal & 0xFF) / 256.0f;            // 0..1
                result = new RGB(hue, saturation, brightness);
            }

        } catch (NumberFormatException e) { }
        return result;
    }

    /**
     * Returns a color from a color factory
     * @param value string representation of the color
     * @return An SWT color object that can be used or <code>null</code> on parse error
     */
    public static Color asColor(String value) {
        if (value == null) return null;
        // look up in the registry
        Color result = str2rgbRegistry.get(value.toLowerCase());
        // if we have it in the registry, return it
        if (result != null) return result;
        // otherwise create an RGB representation from the string
        RGB tempRgb = asRGB(value);
        // if conversion is unsuccessful, return null
        if (tempRgb == null) return null;
        // this might be a problem if we are running on a 256 color display
        return new Color(null, tempRgb);
//        // put it in the registry for later use
//        str2rgbRegistry.put(value.toLowerCase(), tempRgb);
//        // return it as a Color object
//        return str2rgbRegistry.get(value.toLowerCase());
    }

    /**
     * @param value The colors id
     * @return a 16x16 rectangle image filled with the given color (or null, if color id is not valid)
     */
    public static Image asImage(String value) {
        Image img = str2img.get(value);
        if (img != null)
            return img;

        ImageData idata = createColorImageData(value);
        if (idata == null)
            return null;
        // store it for later use
        str2img.put(value, ImageDescriptor.createFromImageData(idata).createImage());
        return str2img.get(value);
    }

	/**
	 * Returns a "good" dark color.
	 */
    public static Color getGoodDarkColor(int i) {
		return goodDarkColors[i % goodDarkColors.length];
	}

	/**
	 * Returns a "good" light color.
	 */
    public static Color getGoodLightColor(int i) {
		return goodLightColors[i % goodLightColors.length];
	}

    /**
     * @return All registered color names
     */
    @SuppressWarnings("unchecked")
	public static String[] getColorNames() {
        return (String[])str2rgbRegistry.getKeySet().toArray(new String[] {});
    }

    // helper function to fill the color table
    private static Color addMapping(String name, RGB rgb) {
        str2rgbRegistry.put(name.toLowerCase(), rgb);
        // check if the given RGB is already in the map
        String colName = rgb2strMap.get(rgb);
        // if the new color name is shorter, use that for the reverse mapping and remove the previous one
        if (colName == null || colName.length() > name.length()) {
            rgb2strMap.remove(rgb);
            rgb2strMap.put(rgb, name);
        }
        return new Color(null, rgb);
    }

    /**
     * Creates and returns the color image data for the
     * RGB value. which is a 16 pixel square.
     *
     * @param colorId the colorId
     */
    private static ImageData createColorImageData(String colorId) {

        int size = 11;
        int indent = 0;
        int rightpadding = 4;
        int extent = 15;

        int width = indent + size + rightpadding;
        int height = extent;

        int xoffset = indent;
        int yoffset = (height - size) / 2;

        RGB black = new RGB(0, 0, 0);
        RGB color = ColorFactory.asRGB(colorId);
        if (color == null)
            return null;

        PaletteData dataPalette
            = new PaletteData(new RGB[] { black, black, color });
        ImageData data = new ImageData(width, height, 4, dataPalette);
        data.transparentPixel = 0;

        int end = size - 1;
        for (int y = 0; y < size; y++) {
            for (int x = 0; x < size; x++) {
                if (x == 0 || y == 0 || x == end || y == end) {
                    data.setPixel(x + xoffset, y + yoffset, 1);
                }
                else {
                    data.setPixel(x + xoffset, y + yoffset, 2);
                }
            }
        }

        return data;
    }
}
