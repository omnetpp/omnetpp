package com.swtworkbench.community.xswt.dataparser;

import java.util.HashMap;
import java.util.Iterator;

import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Widget;

import com.swtworkbench.community.xswt.XSWTException;

public final class DataParser {
    private static HashMap dataParsers = new HashMap();

    private static HashMap disposableData = new HashMap();

    public static void dispose() {
        Iterator cit = disposableData.values().iterator();
        while (cit.hasNext()) {
            Object obj = cit.next();
            if (obj instanceof Color)
                ((Color) obj).dispose();
            else if (obj instanceof Font)
                ((Font) obj).dispose();
            else if (obj instanceof Image)
                ((Image) obj).dispose();
            else
                obj = null;
        }
        disposableData.clear();
    }

    public static void registerDataParser(Class klass, IDataParser dataParser) {
        dataParsers.put(klass, dataParser);
    }

    public static Object parse(String value, Class klass) throws XSWTException {
        IDataParser parser = (IDataParser) dataParsers.get(klass);
        if ((parser == null) && (Widget.class.isAssignableFrom(klass)))
            parser = (IDataParser) dataParsers.get(Widget.class);
        if (parser != null) {
            Object obj = parser.parse(value);
            if (parser.isResourceDisposeRequired()) {
                disposableData.put(klass, obj);
            }
            return obj;
        }
        throw new XSWTException("Unable to find a parser for type: " + klass.getName());
    }
}