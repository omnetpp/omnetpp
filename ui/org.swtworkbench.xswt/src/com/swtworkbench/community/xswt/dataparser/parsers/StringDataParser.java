package com.swtworkbench.community.xswt.dataparser.parsers;

import com.swtworkbench.community.xswt.XSWTException;
import com.swtworkbench.community.xswt.dataparser.NonDisposableDataParser;

public class StringDataParser extends NonDisposableDataParser {
    public Object parse(String source) throws XSWTException {
        return source;
    }
}