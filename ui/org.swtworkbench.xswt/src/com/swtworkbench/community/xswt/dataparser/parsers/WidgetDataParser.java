package com.swtworkbench.community.xswt.dataparser.parsers;

import java.util.HashMap;
import java.util.Map;

import com.swtworkbench.community.xswt.XSWTException;
import com.swtworkbench.community.xswt.dataparser.NonDisposableDataParser;

@SuppressWarnings({ "rawtypes", "unchecked" })
public class WidgetDataParser extends NonDisposableDataParser {
    private Map map = new HashMap();

    public void put(String id, Object obj) {
        this.map.put(id, obj);
    }

    public Object get(String id) {
        return this.map.get(id);
    }

    public Map getWidgetMap() {
        return this.map;
    }

    public Object parse(String source) throws XSWTException {
        Object result = null;
        result = this.map.get(source);
        if (result == null) {
            throw new XSWTException("Undefined ID:" + source);
        }
        return result;
    }
}