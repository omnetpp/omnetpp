package com.swtworkbench.community.xswt.dataparser.parsers;

import com.swtworkbench.community.xswt.XSWTException;
import com.swtworkbench.community.xswt.dataparser.NonDisposableDataParser;

public class FloatDataParser extends NonDisposableDataParser {
    public Object parse(String source) throws XSWTException {
        Float result = null;
        try {
            float intermediate = Float.parseFloat(source);
            result = Float.valueOf(intermediate);
        }
        catch (Exception e) {
            throw new XSWTException(e);
        }
        return result;
    }
}