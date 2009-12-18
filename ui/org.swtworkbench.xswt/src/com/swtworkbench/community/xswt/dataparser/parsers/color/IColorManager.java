package com.swtworkbench.community.xswt.dataparser.parsers.color;

import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.RGB;
import org.eclipse.swt.widgets.Display;

public abstract interface IColorManager {
    public abstract Color getColor(Display paramDisplay, RGB paramRGB);

    public abstract void dispose();
}