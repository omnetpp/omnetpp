package com.swtworkbench.community.xswt.dataparser.parsers;

import com.swtworkbench.community.xswt.XSWTException;
import com.swtworkbench.community.xswt.dataparser.NonDisposableDataParser;
import org.eclipse.swt.widgets.Display;

public class DeviceDataParser extends NonDisposableDataParser {
    public Object parse(String source) throws XSWTException {
        if (source.equalsIgnoreCase("null")) {
            return Display.getCurrent();
        }

        return Display.getCurrent();
    }
}