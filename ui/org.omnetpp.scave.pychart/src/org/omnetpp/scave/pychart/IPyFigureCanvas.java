package org.omnetpp.scave.pychart;

import java.util.ArrayList;
import java.util.HashMap;

/**
 * This is an interface to the our custom FigureCanvas, implemented in Python.
 * 
 * @author attila
 */
public interface IPyFigureCanvas {

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

    void setWidget(IPlotWidget w);

    void performAction(String action);

    String exportFigure(String filename);

    HashMap<String, ArrayList<String>> getSupportedFiletypes();

    String getDefaultFiletype();
}
