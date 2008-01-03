package org.omnetpp.common.util;

import org.eclipse.jface.resource.DataFormatException;
import org.eclipse.jface.resource.StringConverter;
import org.eclipse.swt.graphics.FontData;
import org.eclipse.swt.graphics.RGB;
import org.eclipse.swt.widgets.Display;

/**
 * Some useful conversion between fonts, colors, strings, etc.
 *
 * @author tomi
 */
public class Converter {
	
	public static java.awt.Font fontdataToAwtfont(FontData fontdata) {
		if (fontdata == null)
			return null;
		return new java.awt.Font(fontdata.getName(), fontdata.getStyle(), fontdata.getHeight());
	}
	
	public static FontData awtfontToFontdata(java.awt.Font font) {
		if (font == null)
			return null;
		return new FontData(font.getName(), font.getSize(), font.getStyle());
	}
	
	public static org.eclipse.swt.graphics.Font fontdataToSwtfont(FontData fontdata) {
		if (fontdata == null)
			return null;
		return new org.eclipse.swt.graphics.Font(Display.getDefault(), fontdata);
	}
	
	public static FontData swtfontToFontdata(org.eclipse.swt.graphics.Font font) {
		if (font == null)
			return null;
		return font.getFontData()[0];
	}
	
	public static java.awt.Font stringToAwtfont(String font) {
		return fontdataToAwtfont(stringToFontdata(font));
	}
	
	public static org.eclipse.swt.graphics.Font stringToSwtfont(String font) {
		return fontdataToSwtfont(stringToFontdata(font));
	}
	
	public static <T extends Enum<T>> T stringToEnum(String value, Class<T> enumType) {
		if (value == null)
			return null;
		try {
			return Enum.valueOf(enumType, value);
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
		if (fontdata == null)
			return null;
		return StringConverter.asString(fontdata);
	}
	
	public static FontData stringToFontdata(String value) {
		if (value == null)
			return null;
		try {
			return StringConverter.asFontData(value);
		} catch (DataFormatException e) {
			return null;
		}
	}
	
	public static String booleanToString(Boolean value) {
		return value != null ? StringConverter.asString(value) : null;
	}
	
	public static Boolean stringToBoolean(String value) {
		if (value == null)
			return null;
		try {
			return StringConverter.asBoolean(value);
		} catch (DataFormatException e) {
			return null;
		}
	}
	
	public static String doubleToString(Double value) {
		return value != null ? StringConverter.asString(value) : null;
	}
	
	public static Double stringToDouble(String value) {
		if (value == null)
			return null;
		try {
			return StringConverter.asDouble(value);
		} catch (DataFormatException e) {
			return null;
		}
	}
	
	public static String integerToString(Integer value) {
		return value != null ? StringConverter.asString(value) : null;
	}
	
	public static Integer stringToInteger(String value) {
		if (value == null)
			return null;
		try {
			return StringConverter.asInt(value);
		} catch (DataFormatException e) {
			return null;
		}
	}
	
	public static String rgbToString(RGB value) {
		if (value == null)
			return null;
		return String.format("#%02X%02X%02X", value.red, value.green, value.blue);
	}
	
	public static RGB stringToRGB(String value) {
		if (value == null || value.length() != 7 || value.charAt(0) != '#')
			return null;
		try {
			int red = Integer.parseInt(value.substring(1,3), 16);
			int green = Integer.parseInt(value.substring(3,5), 16);
			int blue = Integer.parseInt(value.substring(5,7), 16);
			return new RGB(red, green, blue);
		}
		catch (NumberFormatException e) {
			return null;
		}
	}
}
