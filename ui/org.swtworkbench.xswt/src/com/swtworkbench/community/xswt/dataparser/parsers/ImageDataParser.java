package com.swtworkbench.community.xswt.dataparser.parsers;

import java.net.URL;
import java.util.StringTokenizer;

import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Display;

import com.swtworkbench.community.xswt.ClassBuilder;
import com.swtworkbench.community.xswt.XSWTException;
import com.swtworkbench.community.xswt.dataparser.DisposableDataParser;
import com.swtworkbench.community.xswt.metalogger.Logger;

public class ImageDataParser extends DisposableDataParser {
    public Object parse(String source) throws XSWTException {
        StringTokenizer stringTokenizer = new StringTokenizer(source, " \t\r\n");
        String classname = stringTokenizer.nextToken();
        String relativePath = stringTokenizer.nextToken();
        Image result = null;
        try {
            Class relativeClass = ClassBuilder.getDefault().getClass(classname);
            URL imageLocation = relativeClass.getResource(relativePath);
            result = new Image(Display.getCurrent(), imageLocation.openStream());
        }
        catch (Exception e) {
            Logger.log().error(e, "Unable to load Image");
        }
        return result;
    }
}