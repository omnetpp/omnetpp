/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.custom.BusyIndicator;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.charting.PlotViewerBase;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.python.MatplotlibChartViewer;

/**
 * Copy chart contents to the clipboard.
 */
public class CopyChartImageToClipboardAction extends AbstractScaveAction {
    public CopyChartImageToClipboardAction() {
        setText("Copy Image to Clipboard");
        setToolTipText("Copy Chart Image to Clipboard");
        setImageDescriptor(ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_COPY));
    }

    @Override
    protected void doRun(ScaveEditor editor, ISelection selection) {

        // TODO: lift copyImageToClipboard to the common ViewerBase class, use that instead of dispatching here

        final PlotViewerBase nativeChart = editor.getActivePlotViewer();

        if (nativeChart != null) {
            BusyIndicator.showWhile(Display.getDefault(), new Runnable() {
                public void run() {
                    nativeChart.copyImageToClipboard();
                }
            });
        }

        final MatplotlibChartViewer matplotlibChart = editor.getActiveMatplotlibChartViewer();

        if (matplotlibChart != null) {
            BusyIndicator.showWhile(Display.getDefault(), new Runnable() {
                public void run() {
                    matplotlibChart.copyImageToClipboard();
                }
            });
        }

    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        return editor.getActivePlotViewer() != null || editor.getActiveMatplotlibChartViewer() != null;
    }
}
