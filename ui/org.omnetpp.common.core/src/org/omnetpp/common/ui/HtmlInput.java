package org.omnetpp.common.ui;

import java.util.Map;

import org.eclipse.swt.graphics.Image;

/**
 * An HTML page containing images to be displayed as a hover.
 *
 * @author levy
 */
public class HtmlInput {
    private String content;
    private Map<String, Image> imageMap;

    public HtmlInput(String content, Map<String, Image> imageMap) {
        this.content = content;
        this.imageMap = imageMap;
    }

    public String getContent() {
        return content;
    }

    public Map<String, Image> getImageMap() {
        return imageMap;
    }
}