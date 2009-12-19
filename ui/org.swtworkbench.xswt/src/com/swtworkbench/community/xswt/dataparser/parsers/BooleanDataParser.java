package com.swtworkbench.community.xswt.dataparser.parsers;

import com.swtworkbench.community.xswt.XSWTException;
import com.swtworkbench.community.xswt.dataparser.NonDisposableDataParser;

public class BooleanDataParser extends NonDisposableDataParser {
    public Object parse(String source) throws XSWTException {
        if ("true".equals(source))
            return new Boolean(true);
        if ("false".equals(source)) {
            return new Boolean(false);
        }
        throw new XSWTException(source + " is not a valid boolean value");
    }
}