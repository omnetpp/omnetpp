package com.swtworkbench.community.xswt.dataparser.parsers;

import com.swtworkbench.community.xswt.dataparser.NonDisposableDataParser;

public class StringBufferDataParser extends NonDisposableDataParser {
    public Object parse(String source) {
        return new StringBuffer(source);
    }
}