package com.swtworkbench.community.xswt.dataparser.parsers.color;

import java.lang.reflect.Field;
import java.lang.reflect.Modifier;
import java.util.HashMap;
import java.util.Map;

import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.RGB;
import org.eclipse.swt.widgets.Display;

import com.swtworkbench.community.xswt.StyleParser;
import com.swtworkbench.community.xswt.XSWTException;
import com.swtworkbench.community.xswt.dataparser.DataParser;
import com.swtworkbench.community.xswt.dataparser.NonDisposableDataParser;

public class ColorDataParser extends NonDisposableDataParser {
    private static Map mapColors = new HashMap();

    private Display display = null;

    static {
        Field[] fields = CSSColors.class.getDeclaredFields();

        for (int i = 0; i < fields.length; ++i) {
            Field field = fields[i];

            if (RGB.class.equals(field.getType())) {
                int iModifiers = field.getModifiers();

                if ((!(Modifier.isPublic(iModifiers))) || (!(Modifier.isStatic(iModifiers))) || (!(Modifier.isFinal(iModifiers))))
                    continue;
                try {
                    mapColors.put(field.getName(), field.get(null));
                }
                catch (IllegalAccessException localIllegalAccessException) {
                }
            }
        }
    }

    public ColorDataParser() {
        this.display = Display.getDefault();
    }

    public Object parse(String source) {
        RGB rgb = (RGB) mapColors.get(source.trim());
        if (rgb != null) {
            this.isDisposable = true;
            return new Color(this.display, rgb);
        }
        try {
            int intermediate = StyleParser.parse(source);
            this.isDisposable = false;
            return this.display.getSystemColor(intermediate);
        }
        catch (XSWTException localXSWTException1) {
            try {
                this.isDisposable = true;
                return new Color(this.display, (RGB) DataParser.parse(source, RGB.class));
            }
            catch (XSWTException localXSWTException2) {
                this.isDisposable = false;
            }
        }
        return this.display.getSystemColor(3);
    }

    public void setDisplay(Display display) {
        this.display = display;
    }
}