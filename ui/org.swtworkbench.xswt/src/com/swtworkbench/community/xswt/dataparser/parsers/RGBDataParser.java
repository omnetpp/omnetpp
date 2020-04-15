package com.swtworkbench.community.xswt.dataparser.parsers;

import org.eclipse.swt.graphics.RGB;

import com.swtworkbench.community.xswt.XSWTException;
import com.swtworkbench.community.xswt.dataparser.DataParser;
import com.swtworkbench.community.xswt.dataparser.NonDisposableDataParser;

public class RGBDataParser extends NonDisposableDataParser {
    public Object parse(String source) throws XSWTException {
        int[] intArray;
        try {
            intArray = (int[]) DataParser.parse(source, int[].class);
        }
        catch (XSWTException e) {
            throw new XSWTException("Unable to parse int[]", e);
        }
        int length = intArray.length;
        int red = (length >= 1) ? intArray[0] : 0;
        int green = (length >= 2) ? intArray[1] : 0;
        int blue = (length >= 3) ? intArray[2] : 0;

        if (red < 0)
            red = 0;
        else if (red > 255) {
            red = 255;
        }
        if (green < 0)
            green = 0;
        else if (green > 255) {
            green = 255;
        }
        if (blue < 0)
            blue = 0;
        else if (blue > 255) {
            blue = 255;
        }
        return new RGB(red, green, blue);
    }
}