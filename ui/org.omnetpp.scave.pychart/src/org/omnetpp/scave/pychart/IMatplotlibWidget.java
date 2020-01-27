package org.omnetpp.scave.pychart;

import java.util.ArrayList;

/**
 * This is the interface that Matplotlib expects from a MatplotlibWidget.
 *
 * @author attila
 *
 */
public interface IMatplotlibWidget {

    /**
     * This is the main drawing method. Calling it will replace
     * the entire contents of the PlotWidget canvas.
     *
     * Note that this is not used at the moment, as we utilize SHM.
     * */
    void setPixels(byte[] pixels, int w, int h);

    void setSharedMemoryNameAndSize(String name, long size);
    void setPixelsShared(int w, int h);

    /**
     * This is used for updating the canvas partially.
     * It paints on top of the already existing canvas contents.
     * Can be used by MPL cursors, and animations.
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

    void setCursorType(int cursor);

    void setMessage(String s);
    void setWarning(String s);
}