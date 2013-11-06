package org.omnetpp.common.ui;

import java.util.Map;

import org.apache.commons.lang3.StringUtils;
import org.eclipse.swt.graphics.Image;

/**
 * An HTML page containing images to be displayed as a hover.
 * The size constraint parameter is optional.
 *
 * @author levy
 */
public class HtmlHoverInfo extends HoverInfo {
    public HtmlHoverInfo(String content) {
        this(content, null, null);
    }

    public HtmlHoverInfo(String content, SizeConstraint sizeConstraint) {
        this(content, null, sizeConstraint);
    }

    public HtmlHoverInfo(String content, Map<String, Image> imageMap, SizeConstraint sizeConstraint) {
        super(HoverSupport.getHtmlHoverControlCreator(),
              StringUtils.isBlank(content) ? null : new HtmlInput(content, imageMap),
              sizeConstraint);
    }
}