package com.swtworkbench.community.xswt.dataparser.parsers;

import com.swtworkbench.community.xswt.XSWTException;
import com.swtworkbench.community.xswt.dataparser.NonDisposableDataParser;

public class DoubleDataParser extends NonDisposableDataParser {
    public Object parse(String source) throws XSWTException {
        Double result = null;
        try {
            double intermediate = Double.parseDouble(source);
            result = Double.valueOf(intermediate);
        }
        catch (Exception e) {
            throw new XSWTException(e);
        }
        return result;
    }
}