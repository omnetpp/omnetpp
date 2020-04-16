package com.swtworkbench.community.xswt.dataparser.parsers;

import com.swtworkbench.community.xswt.XSWTException;
import com.swtworkbench.community.xswt.dataparser.NonDisposableDataParser;

public class LongDataParser extends NonDisposableDataParser {
    public Object parse(String source) throws XSWTException {
        Long result = null;
        try {
            long intermediate = Long.parseLong(source);
            result = Long.valueOf(intermediate);
        }
        catch (Exception e) {
            throw new XSWTException(e);
        }
        return result;
    }
}