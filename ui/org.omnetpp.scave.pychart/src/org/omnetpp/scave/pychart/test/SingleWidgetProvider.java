package org.omnetpp.scave.pychart.test;

import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.ToolBar;
import org.eclipse.swt.widgets.ToolItem;
import org.omnetpp.scave.pychart.ActionDescription;
import org.omnetpp.scave.pychart.IMatplotlibFigureCanvas;
import org.omnetpp.scave.pychart.IMatplotlibWidget;
import org.omnetpp.scave.pychart.IPlotWidgetProvider;
import org.omnetpp.scave.pychart.MatplotlibWidget;

public class SingleWidgetProvider implements IPlotWidgetProvider {
    Shell shell;
    MatplotlibWidget widget;
    IMatplotlibFigureCanvas pythonCanvas = null;

    public SingleWidgetProvider(Shell shell) {
        this.shell = shell;
    }

    @Override
    public IMatplotlibWidget getWidget(int figureNumber, IMatplotlibFigureCanvas canvas) {
        // Assert.isTrue(number == 1, "Only a single canvas is supported");
        Assert.isTrue(pythonCanvas == null, "A canvas was already created");

        pythonCanvas = canvas;

        widget = new MatplotlibWidget(shell, SWT.NONE, null, canvas);

        widget.setLayoutData(new GridData(SWT.FILL, SWT.FILL, false, true));
        return widget;
    }

    @Override
    public void setPlotActions(int figureNumber, List<ActionDescription> actions) {
        ToolBar toolBar = new ToolBar(shell, SWT.FLAT);
        toolBar.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));

        for (ActionDescription action : actions) {

            if (action.text == null) {
                new ToolItem(toolBar, SWT.SEPARATOR);
            }
            else {
                ToolItem toolItem = new ToolItem(toolBar, SWT.PUSH);
                toolItem.setText(action.text);
                toolItem.setToolTipText(action.tooltipText);
                toolItem.setData(action.methodName);

                toolItem.addSelectionListener(new SelectionListener() {

                    @Override
                    public void widgetSelected(SelectionEvent arg0) {
                        // TODO Auto-generated method stub
                        pythonCanvas.performAction(action.methodName);
                    }

                    @Override
                    public void widgetDefaultSelected(SelectionEvent arg0) {
                        // TODO Auto-generated method stub

                    }
                });
            }
        }

    }

    @Override
    public void updateActiveAction(int figureNumber, String action) {
        // TODO Auto-generated method stub

    }

    @Override
    public void setWarning(String warning) {
        // TODO Auto-generated method stub

    }
}
