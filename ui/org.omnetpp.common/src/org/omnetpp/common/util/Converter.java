/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.util;

import org.eclipse.jface.resource.DataFormatException;
import org.eclipse.jface.resource.StringConverter;
import org.eclipse.swt.graphics.FontData;
import org.eclipse.swt.graphics.RGB;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.color.ColorFactory;

/**
 * Some useful conversion between fonts, colors, strings, etc.
 *
 * @author tomi, andras
 */
public class Converter {

    public static java.awt.Font fontdataToAwtfont(FontData fontdata) {
        return fontdata == null ? null : new java.awt.Font(fontdata.getName(), fontdata.getStyle(), fontdata.getHeight());
    }

    public static FontData awtfontToFontdata(java.awt.Font font) {
        return font == null ? null : new FontData(font.getName(), font.getSize(), font.getStyle());
    }

    public static org.eclipse.swt.graphics.Font fontdataToSwtfont(FontData fontdata) {
        return fontdata == null ? null : new org.eclipse.swt.graphics.Font(Display.getDefault(), fontdata);
    }

    public static FontData swtfontToFontdata(org.eclipse.swt.graphics.Font font) {
        return font == null ? null : font.getFontData()[0];
    }

    public static java.awt.Font stringToAwtfont(String font) {
        return fontdataToAwtfont(stringToFontdata(font));
    }

    public static java.awt.Font stringToOptionalAwtfont(String font) {
        return fontdataToAwtfont(stringToOptionalFontdata(font));
    }

    public static java.awt.Font tolerantStringToOptionalAwtfont(String font) {
        return fontdataToAwtfont(tolerantStringToOptionalFontdata(font));
    }

    public static org.eclipse.swt.graphics.Font stringToSwtfont(String font) {
        return fontdataToSwtfont(stringToFontdata(font));
    }

    public static org.eclipse.swt.graphics.Font stringToOptionalSwtfont(String font) {
        return fontdataToSwtfont(stringToOptionalFontdata(font));
    }

    public static org.eclipse.swt.graphics.Font tolerantStringToOptionalSwtfont(String font) {
        return fontdataToSwtfont(tolerantStringToOptionalFontdata(font));
    }

    public static <T extends Enum<T>> T stringToEnum(String value, Class<T> enumType) {
        for (T option : enumType.getEnumConstants())
            if (option.toString().equalsIgnoreCase(value))
                return option;
        throw new IllegalArgumentException("Invalid value '" + value + "' for " + enumType.getSimpleName());
    }

    public static <T extends Enum<T>> T stringToOptionalEnum(String value, Class<T> enumType) {
        return StringUtils.isEmpty(value) ? null : stringToEnum(value, enumType);
    }

    public static <T extends Enum<T>> T tolerantStringToOptionalEnum(String value, Class<T> enumType) {
        try {
            return stringToOptionalEnum(value, enumType);
        } catch (IllegalArgumentException e) {
            return null;
        }
    }

    /*
     * The rest is from StringConverter.
     * This class cannot inherit from StringConverter, because it has
     * only private constructors :(
     */

    public static String fontdataToString(FontData fontdata) {
        return fontdata == null ? null : StringConverter.asString(fontdata);
    }

    public static FontData stringToFontdata(String value) {
        return StringConverter.asFontData(value);
    }

    public static FontData stringToOptionalFontdata(String value) {
        return StringUtils.isEmpty(value) ? null : StringConverter.asFontData(value);
    }

    public static FontData tolerantStringToOptionalFontdata(String value) {
        try {
            return stringToOptionalFontdata(value);
        } catch (DataFormatException e) {
            return null;
        }
    }

    public static String booleanToString(Boolean value) {
        return value != null ? StringConverter.asString(value) : null;
    }

    public static boolean stringToBoolean(String value) {
        return StringConverter.asBoolean(value);
    }

    public static Boolean stringToOptionalBoolean(String value) {
        return StringUtils.isEmpty(value) ? null : StringConverter.asBoolean(value);
    }

    public static Boolean tolerantStringToOptionalBoolean(String value) {
        try {
            return stringToOptionalBoolean(value);
        } catch (DataFormatException e) {
            return null;
        }
    }

    public static String floatToString(Float value) {
        return value != null ? StringConverter.asString(value) : null;
    }

    public static String doubleToString(Double value) {
        return value != null ? StringConverter.asString(value) : null;
    }

    public static float stringToFloat(String value) {
        return StringConverter.asFloat(value);
    }

    public static Float stringToOptionalFloat(String value) {
        return StringUtils.isEmpty(value) ? null : StringConverter.asFloat(value);
    }

    public static Float tolerantStringToOptionalFloat(String value) {
        try {
            return stringToOptionalFloat(value);
        } catch (DataFormatException e) {
            return null;
        }
    }

    public static double stringToDouble(String value) {
        return StringConverter.asDouble(value);
    }

    public static Double stringToOptionalDouble(String value) {
        return StringUtils.isEmpty(value) ? null : StringConverter.asDouble(value);
    }

    public static Double tolerantStringToOptionalDouble(String value) {
        try {
            return stringToOptionalDouble(value);
        } catch (DataFormatException e) {
            return null;
        }
    }

    public static String integerToString(Integer value) {
        return value != null ? StringConverter.asString(value) : null;
    }

    public static int stringToInteger(String value) {
        return StringConverter.asInt(value);
    }

    public static Integer stringToOptionalInteger(String value) {
        return StringUtils.isEmpty(value) ? null : StringConverter.asInt(value);
    }

    public static Integer tolerantStringToOptionalInteger(String value) {
        try {
            return stringToOptionalInteger(value);
        } catch (DataFormatException e) {
            return null;
        }
    }

    public static String rgbToString(RGB value) {
        return value == null ? null : String.format("#%02X%02X%02X", value.red, value.green, value.blue);
    }

    public static RGB stringToRGB(String value) {
        return ColorFactory.asRGBStrict(value);
    }

    public static RGB stringToOptionalRGB(String value) {
        return StringUtils.isEmpty(value) ? null : stringToRGB(value);
    }

    public static RGB tolerantStringToOptionalRGB(String value) {
        try {
            return stringToOptionalRGB(value);
        } catch (NumberFormatException|DataFormatException e) {
            return null;
        }
    }

    public static String objectToString(Object value) {
        if (value == null)
            return null;
        else if (value instanceof FontData)
            return fontdataToString((FontData)value);
        else if (value instanceof Boolean)
            return booleanToString((Boolean)value);
        else if (value instanceof Double)
            return doubleToString((Double)value);
        else if (value instanceof Integer)
            return integerToString((Integer)value);
        else if (value instanceof RGB)
            return ColorFactory.asString((RGB)value);

        return value.toString();
    }
}
