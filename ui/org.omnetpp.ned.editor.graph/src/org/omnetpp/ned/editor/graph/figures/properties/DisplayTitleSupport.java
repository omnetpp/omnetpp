package org.omnetpp.ned.editor.graph.figures.properties;

import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Image;

public interface DisplayTitleSupport extends DisplayNameSupport {
    public void setDefaultShape(Image img,
    		String shape, int shapeWidth, int shapeHeight, 
            Color shapeFillColor, Color shapeBorderColor,int shapeBorderWidth);

}
