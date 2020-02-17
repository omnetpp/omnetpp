package org.omnetpp.scave.charting;

import org.eclipse.jface.util.IPropertyChangeListener;
import org.omnetpp.common.canvas.ZoomableCachingCanvas;
import org.omnetpp.scave.charting.dataset.IDataset;

public interface IPlotViewer {

    ZoomableCachingCanvas getCanvas();

    void setDataset(IDataset dataset);

    void setProperty(String key, String value);

    void addPropertyChangeListener(IPropertyChangeListener listener);
    void removePropertyChangeListener(IPropertyChangeListener listener);

    IPlotSelection getSelection();

    void addChartSelectionListener(IChartSelectionListener listener);
    void removeChartSelectionListener(IChartSelectionListener listener);

    void setStatusText(String text);

    int getMouseMode();
    void setMouseMode(int mode);
}
