package com.swtworkbench.community.xswt.dataparser.parsers;

import java.util.StringTokenizer;

import com.swtworkbench.community.xswt.XSWTException;
import com.swtworkbench.community.xswt.dataparser.NonDisposableDataParser;

public class IntArrayDataParser extends NonDisposableDataParser {
    public Object parse(String source) throws XSWTException {
        StringTokenizer stringTokenizer = new StringTokenizer(source, " \t\r\n");
        int tokens = stringTokenizer.countTokens();
        int[] intArray = new int[tokens];

        for (int i = 0; i < tokens; ++i) {
            try {
                intArray[i] = Integer.parseInt(stringTokenizer.nextToken().trim());
            }
            catch (NumberFormatException e) {
                throw new XSWTException(e);
            }
        }

        return intArray;
    }
}