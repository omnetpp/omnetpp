package org.omnetpp.scave.pychart;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

/**
 * This is an interface to the our custom FigureCanvas, implemented in Python.
 * 
 * @author attila
 */
public interface IMatplotlibFigureCanvas {

    // Declaration of event handlers:
    //
    // These are normally called by the SWT event handlers of a PlotWidget,
    // and are passed over to the Python side, to Matplotlib, which then does
    // however it pleases (redraws, calls user-defined handlers, and so on).
    void enterEvent(int x, int y);

    void leaveEvent();

    void mouseMoveEvent(int x, int y);

    void mousePressEvent(int x, int y, int button);

    void mouseReleaseEvent(int x, int y, int button);

    void mouseDoubleClickEvent(int x, int y, int button);

    void resizeEvent(int width, int height);

    /*
     * // TODO implement scroll and keypress event handlers def wheelEvent(self,
     * event): x, y = self.mouseEventCoords(event) # from QWheelEvent::delta doc if
     * event.pixelDelta().x() == 0 and event.pixelDelta().y() == 0: steps =
     * event.angleDelta().y() / 120 else: steps = event.pixelDelta().y() if steps:
     * FigureCanvasBase.scroll_event(self, x, y, steps, guiEvent=event)
     * 
     * def keyPressEvent(self, event): FigureCanvasBase.key_press_event(self, key)
     * 
     * def keyReleaseEvent(self, event): FigureCanvasBase.key_release_event(self,
     * key)
     * 
     */

    void setWidget(IMatplotlibWidget w);

    void performAction(String action);

    String exportFigure(String filename);

    HashMap<String, ArrayList<String>> getSupportedFiletypes();

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
