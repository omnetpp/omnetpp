package org.omnetpp.common.ui;

import java.util.Map;

import org.apache.commons.lang3.StringUtils;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.common.util.HTMLUtils.IImageProvider;

/**
 * An HTML page containing images to be displayed as a hover.
 * The size constraint parameter is optional.
 *
 * @author levy
 */
public class HtmlHoverInfo extends HoverInfo {
    public HtmlHoverInfo(String content) {
        this(content, (Map<String, Image>)null, null);
    }

    public HtmlHoverInfo(String content, SizeConstraint sizeConstraint) {
        this(content, (Map<String, Image>)null, sizeConstraint);
    }

    public HtmlHoverInfo(String content, IImageProvider imageProvider) {
        this(content, imageProvider, null);
    }

    public HtmlHoverInfo(String content, Map<String, Image> imageMap) {
        this(content, imageMap, null);
    }

    public HtmlHoverInfo(String content, IImageProvider imageProvider, SizeConstraint sizeConstraint) {
        super(HoverSupport.getHtmlHoverControlCreator(),
              StringUtils.isBlank(content) ? null : new HtmlInput(content, imageProvider),
              sizeConstraint);
    }

    public HtmlHoverInfo(String content, Map<String, Image> imageMap, SizeConstraint sizeConstraint) {
        this(content, (imageName) -> imageMap == null ? null : imageMap.get(imageName), sizeConstraint);
    }
}