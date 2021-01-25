package org.omnetpp.common.ui;

import org.omnetpp.common.util.HTMLUtils.IImageProvider;

/**
 * An HTML page containing images to be displayed as a hover.
 *
 * @author levy
 */
public class HtmlInput {
    private String content;
    private IImageProvider imageProvider;

    public HtmlInput(String content, IImageProvider imageProvider) {
        this.content = content;
        this.imageProvider = imageProvider;
    }

    public String getContent() {
        return content;
    }

    public IImageProvider getImageProvider() {
        return imageProvider;
    }
}