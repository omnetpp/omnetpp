package org.omnetpp.scave.pychart.test;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;
import org.omnetpp.scave.pychart.ActionDescription;
import org.omnetpp.scave.pychart.IMatplotlibWidget;
import org.omnetpp.scave.pychart.IPlotWidgetProvider;
import org.omnetpp.scave.pychart.IMatplotlibFigureCanvas;
import org.omnetpp.scave.pychart.MatplotlibWidget;
import org.omnetpp.scave.pychart.PythonProcess;

public class MultiWidgetProvider implements IPlotWidgetProvider {
    Shell parentShell;
    PythonProcess pythonProcess;

    class FigureData {
        public FigureData(Shell shell, IMatplotlibWidget widget, Composite toolbar, IMatplotlibFigureCanvas canvas) {
            this.shell = shell;
            this.widget = widget;
            this.toolbar = toolbar;
            this.pythonCanvas = canvas;
        }

        public Shell shell;
        public IMatplotlibWidget widget;
        public Composite toolbar;
        public IMatplotlibFigureCanvas pythonCanvas;
    }

    Map<Integer, FigureData> figures = new HashMap<Integer, FigureData>();

    public MultiWidgetProvider(Shell shell, PythonProcess pythonProcess) {
        this.parentShell = shell;
        this.pythonProcess = pythonProcess;
    }

    @Override
    public IMatplotlibWidget getWidget(int figureNumber, IMatplotlibFigureCanvas canvas) {

        Display.getDefault().syncExec(() -> {
            if (figures.containsKey(figureNumber))
                return;

            Shell shell = new Shell(parentShell, SWT.RESIZE | SWT.CLOSE);
            shell.setText("Figure " + figureNumber);

            shell.setLayout(new GridLayout(1, true));
            MatplotlibWidget widget = new MatplotlibWidget(shell, SWT.NONE, pythonProcess, canvas);
            widget.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));

            Composite toolbar = new Composite(shell, SWT.NONE);
            toolbar.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
            toolbar.setLayout(new GridLayout(10, false));

            shell.open();
            figures.put(figureNumber, new FigureData(shell, widget, toolbar, canvas));
        });

        return figures.get(figureNumber).widget;
    }

    @Override
    public void setPlotActions(int figureNumber, List<ActionDescription> actions) {
        Display.getDefault().asyncExec(() -> {
            Composite toolbar = figures.get(figureNumber).toolbar;

            for (ActionDescription action : actions) {
                if (action.text == null) // separators
                    continue;

                Button button = new Button(toolbar, SWT.PUSH);
                button.setText(action.text);

                button.setLayoutData(new GridData(SWT.FILL, SWT.FILL, false, false));
                button.addSelectionListener(new SelectionListener() {

                    @Override
                    public void widgetSelected(SelectionEvent arg0) {
                        figures.get(figureNumber).pythonCanvas.performAction(action.methodName);
                    }

                    @Override
                    public void widgetDefaultSelected(SelectionEvent arg0) {
                    }
                });
            }

            figures.get(figureNumber).shell.layout();
        });
    }

    @Override
    public void updateActiveAction(int figureNumber, String action) {

    }

    @Override
    public void setWarning(String warning) {
        // TODO Auto-generated method stub

    }
}
