package com.swtworkbench.community.xswt.editors;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.RGB;
import org.eclipse.swt.widgets.Display;

@SuppressWarnings({ "rawtypes", "unchecked" })
public class ColorManager {
    protected Map fColorTable = new HashMap(10);

    public void dispose() {
        Iterator e = this.fColorTable.values().iterator();
        while (e.hasNext())
            ((Color) e.next()).dispose();
    }

    public Color getColor(RGB rgb) {
        Color color = (Color) this.fColorTable.get(rgb);
        if (color == null) {
            color = new Color(Display.getCurrent(), rgb);
            this.fColorTable.put(rgb, color);
        }
        return color;
    }
}