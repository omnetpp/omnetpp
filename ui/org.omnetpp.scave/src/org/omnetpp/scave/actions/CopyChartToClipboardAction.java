/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.swt.custom.BusyIndicator;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.charting.ChartViewer;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.python.MatplotlibChartViewer;

/**
 * Copy chart contents to the clipboard.
 */
public class CopyChartToClipboardAction extends AbstractScaveAction {
    public CopyChartToClipboardAction() {
        setText("Copy Image to Clipboard");
        setToolTipText("Copy Chart Image to Clipboard");
        setImageDescriptor(ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_COPY));
    }

    @Override
    protected void doRun(ScaveEditor editor, IStructuredSelection selection) {
        final ChartViewer nativeChart = editor.getActiveChartViewer();

        if (nativeChart != null) {
            BusyIndicator.showWhile(Display.getDefault(), new Runnable() {
                public void run() {
                    nativeChart.copyToClipboard();
                }
            });
        }

        final MatplotlibChartViewer matplotlibChart = editor.getActiveMatplotlibChartViewer();

        if (matplotlibChart != null) {
            BusyIndicator.showWhile(Display.getDefault(), new Runnable() {
                public void run() {
                    matplotlibChart.copyToClipboard();
                }
            });
        }

    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
        return editor.getActiveChartViewer() != null || editor.getActiveMatplotlibChartViewer() != null;
    }
}
