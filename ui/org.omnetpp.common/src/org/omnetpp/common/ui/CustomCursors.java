/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
 *--------------------------------------------------------------*/

package org.omnetpp.common.ui;

import org.eclipse.swt.graphics.Cursor;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.common.image.ImageFactory;

/**
 * Custom cursors.
 *
 * @author tomi
 */
public class CustomCursors {

    /** Zoom in cursor */
    public static final Cursor ZOOMIN;

    /** Zoom out cursor */
    public static final Cursor ZOOMOUT;

    /**
     * Dragging hand cursor (note: stock "hand" cursor is no good, because it's
     * different on Linux (dragging hand) and Windows (finger-pointing hand)
     */
    public static final Cursor DRAGGER;

    static {
        ZOOMIN = createCursor(ImageFactory.CURSOR_IMAGE_ZOOMIN, 4, 4);
        ZOOMOUT = createCursor(ImageFactory.CURSOR_IMAGE_ZOOMOUT, 4, 4);
        DRAGGER = createCursor(ImageFactory.CURSOR_IMAGE_DRAGGER, 7, 3);
    }

    private static Cursor createCursor(String imageName, int hotspotX, int hotspotY) {
        Image image = ImageFactory.global().getImage(imageName);
        return new Cursor(null, image.getImageData(), hotspotX, hotspotY);
    }
}
