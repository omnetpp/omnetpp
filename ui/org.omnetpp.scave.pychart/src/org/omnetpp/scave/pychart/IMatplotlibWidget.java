/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.pychart;

import java.util.ArrayList;

/**
 * The Python part of our custom FigureCanvas communicates with the Java (SWT)
 * part through this interface. It has methods to put image content (pixels)
 * on the screen in various ways, to set or clear the tool rectangle, to set
 * the mouse cursor type, and to pass a regular message (used by matplotlib
 * tools) or a warning text to the user.
 */
public interface IMatplotlibWidget {

    /**
     * Replaces he entire contents of the canvas with the raw pixels stored in
     * the passed buffer. Each pixel is an RGBA quartet.
     */
    void setPixels(byte[] pixels, int w, int h);

    /**
     * Asks the canvas implementation to map a shared memory object with the
     * given name and size as "pixel backing storage". Pixels for subsequent
     * frames data should be read from this SHM object.
     */
    void setSharedMemoryNameAndSize(String name, long size);

    /**
     * Signals to the implementation that a new frame was written into the
     * previously selected shared memory region, and that it should be presented
     * to the user. The actual size of the frame might be smaller than the size
     * of the SHM region, because that is recreated only if it is too small, but
     * is usually not shrunk.
     */
    void setPixelsShared(int w, int h);

    /**
     * This is used for updating the canvas partially.
     * It paints on top of (replacing) the already existing canvas contents.
     * Might be used by custom MPL cursors, and animations.
     * */
    void blit(byte[] pixels, int x, int y, int w, int h);

    /**
     * Sets the "selection rectangle" to draw on top of the image.
     * Used when the "zoom" tool is active.
     */
    void setRect(ArrayList<Integer> rect);

    /**
     * Clears the rectangle set by 'setRect'.
     */
    void clearRect();

    /**
     * Sets the mouse cursor type. The cursors are encoded as:
     * HAND = 0, POINTER = 1, SELECT_REGION = 2, MOVE = 3, WAIT = 4
     */
    void setCursorType(int cursor);

    /**
     * Sets the tool message to show to the user. This commonly contains
     * the current navigation mode and data coordinates of the mouse pointer.
     */
    void setMessage(String s);

    /**
     * Sets the warning text to show to the user. This is used usually when
     * the chart was not configured correctly.
     */
    void setWarning(String s);
}