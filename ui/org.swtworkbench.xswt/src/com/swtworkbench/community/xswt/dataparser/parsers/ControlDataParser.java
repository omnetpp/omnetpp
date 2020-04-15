package com.swtworkbench.community.xswt.dataparser.parsers;

import java.util.HashMap;
import java.util.Map;

import org.eclipse.swt.widgets.Control;

import com.swtworkbench.community.xswt.XSWTException;
import com.swtworkbench.community.xswt.dataparser.NonDisposableDataParser;

@SuppressWarnings({"rawtypes", "unchecked"})
public class ControlDataParser extends NonDisposableDataParser {
    private Map map = new HashMap();

    public void put(String id, Object obj) {
        this.map.put(id, obj);
    }

    public Map getControlMap() {
        return this.map;
    }

    public Object parse(String source) throws XSWTException {
        Control result = null;
        result = (Control) this.map.get(source);
        if (result == null) {
            throw new XSWTException("Undefined ID:" + source);
        }
        return result;
    }
}