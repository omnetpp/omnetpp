package com.swtworkbench.community.xswt.dataparser.parsers;

import org.eclipse.swt.graphics.Point;

import com.swtworkbench.community.xswt.XSWTException;
import com.swtworkbench.community.xswt.dataparser.DataParser;
import com.swtworkbench.community.xswt.dataparser.NonDisposableDataParser;

public class PointDataParser extends NonDisposableDataParser {
    public Object parse(String source) throws XSWTException {
        int[] intArray;
        try {
            intArray = (int[]) DataParser.parse(source, int[].class);
        }
        catch (XSWTException e) {
            throw new XSWTException("Unable to parse int[]", e);
        }
        int length = intArray.length;
        return new Point((length >= 1) ? intArray[0] : 0, (length >= 2) ? intArray[1] : 0);
    }
}