/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.pychart;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

/**
 * This is an interface to the Python half our custom matplotlib FigureCanvas.
 *
 * The SWT Canvas (MatplotlibWidget) passes the input events back to matplotlib
 * through this interface.
 *
 * Performing actions (as described by ActionDescription - tool selection and
 * nacigation history: zoom, pan, back, forward, home, etc.); exporting the
 * figure (by matplotlib, in any format supported by it); querying the supported
 * export file formats (png, svg, ps, etc.); and querying and restoring the axis
 * limits (to keep the navigation state the same across subsequent script
 * executions); also happens through this interface.
 */
// TODO implement scroll and keypress event handlers
public interface IMatplotlibFigureCanvas {

    // Declaration of event handlers:
    //
    // These are normally called by the SWT event handlers of a PlotWidget,
    // and are passed over to the Python side, to Matplotlib, which then does
    // however it pleases (redraws, calls user-defined handlers, and so on).


    /** Notifies matplotlib that the mouse pointer entered the canvas area at the
     * given (canvas-local) coordinates, with the origin in the lower left corner. */
    void enterEvent(int x, int y);

    /** Notifies matplotlib that the mouse pointer left the canvas area. */
    void leaveEvent();

    /** Notifies matplotlib that the mouse pointer moved to the given (canvas-local)
     * coordinates (with the origin in the lower left corner) inside the canvas area. */
    void mouseMoveEvent(int x, int y);

    /** Notifies matplotlib that the given mouse button was pressed down at the
     * given (canvas-local) coordinates (with the origin in the lower left corner)
     * inside the canvas area. The button is encoded as: LEFT = 1, MIDDLE = 2,
     * RIGHT = 3, BACK = 8, FORWARD = 9. */
    void mousePressEvent(int x, int y, int button);

    /** Notifies matplotlib that the given mouse button was released at the
     * given (canvas-local) coordinates (with the origin in the lower left corner)
     * inside the canvas area. The button is encoded as: LEFT = 1, MIDDLE = 2,
     * RIGHT = 3, BACK = 8, FORWARD = 9. */
    void mouseReleaseEvent(int x, int y, int button);

    /** Notifies matplotlib that the given mouse button was double-clicked at
     * the given (canvas-local) coordinates (with the origin in the lower left corner)
     * inside the canvas area. The button is encoded as: LEFT = 1, MIDDLE = 2,
     * RIGHT = 3, BACK = 8, FORWARD = 9. */
    void mouseDoubleClickEvent(int x, int y, int button);

    /** Notifies matplotlib that the canvas was resized to the given size. */
    void resizeEvent(int width, int height);

    /** Notifies matplotlib that the canvas was resized to the given size. */
    void performAction(String action);

    /** Save the image currently on the canvas into the the given file. The
     * format to use will be automatically detected by matplotlib from the
     * extension in the filename. */
    void exportFigure(String filename);

    /**
     * Returns a map with Matplotlib's supported graphical image format,
     * with descriptions as keys, and the list of file extensions as values.
     * E.g. "Tagged Image File Format" -> ["tif", "tiff"]
     */
    HashMap<String, ArrayList<String>> getSupportedFiletypes();

    /** Returns the extension of the default filetype when exporting figures. */
    String getDefaultFiletype();

    /**
     * The members are actually Double instances, but we don't use them from
     * Java, only pass them back in to the next Python process.
     * The (flat) list contains 4 elements [xmin, xmax, ymin, ymax] for each
     * axes object ("subplot") of the figure.
     */
    List<Object> getAxisLimits();

    /**
     * Accepts the same kind of List as getAxisLimit returns.
     * If there are more elements than axes in the (now potentially different)
     * figure, the unneeded values are ignored at the end. If there are fewer,
     * the remaining axes objects are untouched.
     */
    void setAxisLimits(List<Object> limits);
}
