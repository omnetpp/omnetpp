package com.swtworkbench.community.xswt.dataparser.parsers;

import com.swtworkbench.community.xswt.StyleParser;
import com.swtworkbench.community.xswt.XSWTException;
import com.swtworkbench.community.xswt.dataparser.NonDisposableDataParser;

public class IntDataParser extends NonDisposableDataParser {
    public Object parse(String source) throws XSWTException {
        Integer result = null;
        try {
            result = Integer.decode(source);
        }
        catch (Exception localException) {
        }
        if (result != null)
            return result;

        int intermediate = StyleParser.parse(source);
        return Integer.valueOf(intermediate);
    }
}