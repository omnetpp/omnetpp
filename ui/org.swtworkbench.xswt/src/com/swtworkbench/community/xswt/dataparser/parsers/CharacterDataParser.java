package com.swtworkbench.community.xswt.dataparser.parsers;

import com.swtworkbench.community.xswt.XSWTException;
import com.swtworkbench.community.xswt.dataparser.NonDisposableDataParser;

public class CharacterDataParser extends NonDisposableDataParser {
    public Object parse(String source) throws XSWTException {
        Character result = null;
        try {
            if (source.length() > 1)
                throw new XSWTException("Length of a char cannot exceed 1: " + source);
            char intermediate = source.charAt(0);
            result = Character.valueOf(intermediate);
        }
        catch (Exception e) {
            throw new XSWTException(e);
        }
        return result;
    }
}