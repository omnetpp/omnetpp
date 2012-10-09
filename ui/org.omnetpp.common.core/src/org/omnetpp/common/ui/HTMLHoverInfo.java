package org.omnetpp.common.ui;

import java.util.Map;

import org.eclipse.swt.graphics.Image;

/**
 * An HTML page containing images to be displayed as a hover.
 * The size constraint parameter is optional.
 *
 * @author levy
 */
public class HTMLHoverInfo {
    private String content;

    private Map<String, Image> imageMap;

    private SizeConstraint sizeConstraint;

    public HTMLHoverInfo(String content) {
        this(content, null, null);
    }

    public HTMLHoverInfo(String content, SizeConstraint sizeConstraint) {
        this(content, null, sizeConstraint);
    }

    public HTMLHoverInfo(String content, Map<String, Image> imageMap, SizeConstraint sizeConstraint) {
        this.content = content;
        this.imageMap = imageMap;
        this.sizeConstraint = sizeConstraint;
    }

    public String getContent() {
        return content;
    }

    public Map<String, Image> getImageMap() {
        return imageMap;
    }

    public SizeConstraint getSizeConstraint() {
        return sizeConstraint;
    }
}