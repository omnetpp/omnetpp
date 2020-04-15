package com.swtworkbench.community.xswt.dataparser.parsers;

import java.util.StringTokenizer;

import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.widgets.Display;

import com.swtworkbench.community.xswt.StyleParser;
import com.swtworkbench.community.xswt.dataparser.DisposableDataParser;

public class FontDataParser extends DisposableDataParser {
    private Display display = null;

    public FontDataParser() {
        this.display = Display.getDefault();
    }

    public Object parse(String source) {
        String name = null;
        int height = 10;
        int style = 0;

        StringTokenizer stringTokenizer = new StringTokenizer(source, ", \t\r\n");

        if (stringTokenizer.hasMoreTokens()) {
            name = stringTokenizer.nextToken().trim();
        }
        if (stringTokenizer.hasMoreTokens()) {
            try {
                height = Integer.parseInt(stringTokenizer.nextToken().trim());
            }
            catch (NumberFormatException localNumberFormatException) {
                height = 10;
            }
        }

        if (stringTokenizer.hasMoreTokens()) {
            try {
                style = StyleParser.parse(stringTokenizer.nextToken().trim());
            }
            catch (Exception localException1) {
                style = 0;
            }
        }
        if (this.display == null)
            this.display = Display.getCurrent();
        Font f = null;
        try {
            f = new Font(this.display, name, height, style);
        }
        catch (Exception localException2) {
            f = this.display.getSystemFont();
        }
        return f;
    }

    public void setDisplay(Display display) {
        this.display = display;
    }
}