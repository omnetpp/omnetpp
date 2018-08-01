/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.ui;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.emf.common.command.CompoundCommand;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.emf.edit.command.SetCommand;
import org.eclipse.emf.edit.domain.EditingDomain;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.ScrolledComposite;
import org.eclipse.swt.events.MouseAdapter;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Canvas;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Layout;
import org.eclipse.ui.forms.widgets.ILayoutExtension;
import org.omnetpp.common.canvas.ZoomableCachingCanvas;
import org.omnetpp.common.ui.FocusManager;
import org.omnetpp.common.ui.LiveTable;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.actions.ClosePageAction;
import org.omnetpp.scave.charting.ChartViewer;
import org.omnetpp.scave.charting.ChartViewerFactory;
import org.omnetpp.scave.charting.ChartUpdater;
import org.omnetpp.scave.charting.IChartView;
import org.omnetpp.scave.charting.properties.ChartProperties;
import org.omnetpp.scave.charting.properties.ChartSheetProperties;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.ScaveEditorContributor;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ChartSheet;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.model.ScaveModelPackage;

//TODO chart context menu: replace "Go to ChartSheet Definition" with "Go to Chart Definition"
//TODO chart context menu: add "Remove From Chart Sheet"
//TODO dblclick on chart: after closing, mouse dragging mode sticks
public class ChartSheetPage extends FormEditorPage {

    private ChartSheet chartSheet; // the underlying model

    private ScrolledComposite scrolledComposite;
    private LiveTable chartsArea;
    private List<ChartUpdater> updaters = new ArrayList<ChartUpdater>();

    public ChartSheetPage(Composite parent, ScaveEditor editor, ChartSheet chartsheet) {
        super(parent, SWT.NONE, editor);
        this.chartSheet = chartsheet;
        initialize();
        
        ScaveEditorContributor contributor = ScaveEditorContributor.getDefault();
        if (chartSheet.isTemporary())
            addToToolbar(contributor.getSaveTempChartSheetAction());
        else
            addToToolbar(contributor.getGotoChartSheetDefinitionAction());
        addSeparatorToToolbar();
        addToToolbar(contributor.getCopyChartToClipboardAction());
        addToToolbar(contributor.getExportToSVGAction());
        addSeparatorToToolbar();
        addToToolbar(contributor.getSwitchChartToPanModeAction());
        addToToolbar(contributor.getSwitchChartToZoomModeAction());
        addSeparatorToToolbar();
        addToToolbar(contributor.getZoomToFitAction());
        addToToolbar(contributor.getHZoomInAction());
        addToToolbar(contributor.getHZoomOutAction());
        addToToolbar(contributor.getVZoomInAction());
        addToToolbar(contributor.getVZoomOutAction());
        addSeparatorToToolbar();
        addToToolbar(contributor.getRefreshChartAction());
        addSeparatorToToolbar();
        addToToolbar(new ClosePageAction());
        
        //TODO add removeFromchartSheetAction();
        
        //TODO contributor.refreshActions()! automatically, inside ScaveEditor.openClosablePage or so.
    }

    private void initialize() {
        // set up UI
        setPageTitle(getChartSheetName(chartSheet));
        setFormTitle(getChartSheetName(chartSheet));
//        getHeader().addMouseListener(new MouseAdapter() {
//            // mouse click on the title selects the chart sheet in the model
//            public void mouseUp(MouseEvent e) {
//                chartsArea.setSelection(new Control[0]);
//                scaveEditor.setSelection(new StructuredSelection(chartSheet));
//            }
//        });

        getContent().setLayout(new FillLayout());

        scrolledComposite = new ScrolledComposite(getContent(), SWT.V_SCROLL | SWT.H_SCROLL);
        scrolledComposite.setExpandHorizontal(true);
        scrolledComposite.setExpandVertical(true);

        chartsArea = new LiveTable(scrolledComposite, SWT.DOUBLE_BUFFERED);
        chartsArea.setBackground(getBackground());
        chartsArea.setLayout(new GridLayout(ChartSheetProperties.DEFAULT_COLUMN_COUNT, true));

        chartsArea.addSelectionChangedListener(new ISelectionChangedListener() {
            public void selectionChanged(SelectionChangedEvent event) {
                scaveEditor.setSelection(event.getSelection());
            }
        });

        chartsArea.addChildOrderChangedListener(new LiveTable.IChildOrderChangedListener() {
            public void childOrderChanged(LiveTable table) {
                if (table == chartsArea && !chartsArea.isDisposed())
                    handleChildOrderChanged();
            }
        });

        scrolledComposite.setContent(chartsArea);

        // set up contents
        for (Chart chart : chartSheet.getCharts())
            addChartView(chart);

        for (Property property : chartSheet.getProperties())
            setChartSheetProperty(property.getName(), property.getValue());

        // ensure that focus gets restored correctly after user goes somewhere else and then comes back
        setFocusManager(new FocusManager(this));

        scrolledComposite.setMinSize(chartsArea.computeSize(SWT.DEFAULT, SWT.DEFAULT));

    }

    public ChartSheet getChartSheet() {
        return chartSheet;
    }

    @Override
    @SuppressWarnings("unchecked")
    public void updatePage(Notification notification) {
        Object notifier = notification.getNotifier();

        if (notifier == chartSheet) {
            switch (notification.getFeatureID(ChartSheet.class)) {
                case ScaveModelPackage.CHART_SHEET__NAME:
                    setPageTitle(getChartSheetName(chartSheet));
                    setFormTitle(getChartSheetName(chartSheet));
                    break;
                case ScaveModelPackage.CHART_SHEET__CHARTS:
                    synchronize();
                    updateLayout();
                    break;
                case ScaveModelPackage.CHART_SHEET__PROPERTIES:
                    Property property;
                    switch (notification.getEventType()) {
                        case Notification.ADD:
                            property = (Property)notification.getNewValue();
                            setChartSheetProperty(property.getName(), property.getValue());
                            break;
                        case Notification.REMOVE:
                            property = (Property)notification.getOldValue();
                            setChartSheetProperty(property.getName(), null);
                            break;
                        case Notification.ADD_MANY:
                            for (Property prop : (List<Property>)notification.getNewValue())
                                setChartSheetProperty(prop.getName(), prop.getValue());
                            break;
                        case Notification.REMOVE_MANY:
                            for (Property prop : (List<Property>)notification.getOldValue())
                                setChartSheetProperty(prop.getName(), null);
                            break;
                    }
                    break;
            }
        }
        else if (notifier instanceof Property) {
            Property property = (Property)notifier;
            switch (notification.getEventType()) {
                case Notification.SET:
                    setChartSheetProperty(property.getName(), (String)notification.getNewValue());
                    break;
            }
        }
        else
            updateCharts();
    }

    public void updateCharts() {
        for (ChartUpdater updater : updaters)
            updater.updateDataset();
    }

    public Composite getChartSheetComposite() {
        return chartsArea;
    }

    private void addChartUpdater(Chart chart, IChartView view) {
        //updaters.add(new ChartUpdater(chart, view, scaveEditor.getResultFileManager()));
    }

    private void removeChartUpdater(Chart chart) {
        for (Iterator<ChartUpdater> iterator = updaters.iterator(); iterator.hasNext(); ) {
            ChartUpdater updater = iterator.next();
            if (updater.getChart().equals(chart)) {
                iterator.remove();
                break;
            }
        }
    }

    private Chart findChart(Control view) {
        for (ChartUpdater updater : updaters)
            if (updater.getChartView().getCanvas().equals(view))
                return updater.getChart();
        return null;
    }

    private IChartView findChartView(Control canvas) {
        for (ChartUpdater updater : updaters)
            if (updater.getChartView().getCanvas().equals(canvas))
                return updater.getChartView();
        return null;
    }

    private IChartView findChartView(Chart chart) {
        for (ChartUpdater updater : updaters)
            if (updater.getChart().equals(chart))
                return updater.getChartView();
        return null;
    }

    private IChartView addChartView(final Chart chart) {
        /*IChartView view = ChartFactory.createChart(chartsArea, chart, scaveEditor.getResultFileManager());
        Assert.isNotNull(view);*/
        
        IChartView view = null;

        Control viewControl = view.getCanvas();
        GridData gridData = new GridData(SWT.FILL, SWT.FILL, true, true);
        gridData.minimumWidth = ChartSheetProperties.DEFAULT_MIN_CHART_WIDTH;
        gridData.minimumHeight = ChartSheetProperties.DEFAULT_MIN_CHART_HEIGHT;
        viewControl.setLayoutData(gridData);
        chartsArea.configureChild(viewControl);
        addChartUpdater(chart, view);
        //MenuManager menuManager = new ChartMenuManager(chart, scaveEditor);
        //viewControl.setMenu(menuManager.createContextMenu(viewControl));
        // hide legend
        view.setProperty(ChartProperties.PROP_DISPLAY_LEGEND, "false");

        ScaveEditorContributor contributor = ScaveEditorContributor.getDefault();
        if (contributor != null)
            contributor.registerChart(view);

        viewControl.addMouseListener(new MouseAdapter() {
            // double click in the view opens the chart in a separate page
            @Override
            public void mouseDoubleClick(MouseEvent e) {
                scaveEditor.openChart(chart);
            }

            // mouse click on the view selects the chart object in the model
            // We must use the mouse down event to set a new selection otherwise Actions in
            // the context menu will not see the new selection because they are
            // displayed right after mause down (and befoe mouse up)
            @Override
            public void mouseDown(MouseEvent e) {
                scaveEditor.setSelection(new StructuredSelection(chart));
            }
        });

        if (viewControl instanceof ChartViewer)
        setDisplayChartDetails((ChartViewer)viewControl, ChartSheetProperties.DEFAULT_DISPLAY_CHART_DETAILS);
        return view;
    }

    private void removeChartView(Chart chart) {
        IChartView view = findChartView(chart);
        removeChartUpdater(chart);
        if (view != null) {
            Canvas canvas = view.getCanvas();
            if (!canvas.isDisposed()) {
                canvas.dispose();
            }
        }
    }

    private void synchronize() {
        List<Control> currentChildren = new ArrayList<Control>();
        for (Control child : chartsArea.getChildren())
            if (child instanceof ZoomableCachingCanvas)
                currentChildren.add(child);

        List<Chart> charts = new ArrayList<Chart>(chartSheet.getCharts());
        List<Control> children = new ArrayList<Control>(charts.size());

        for (Chart chart : charts) {
            IChartView view = findChartView(chart);
            if (view == null)
                view = addChartView(chart);
            children.add(view.getCanvas());
        }

        for (Control child : currentChildren) {
            if (!children.contains(child)) {
                Chart chart = findChart(child);
                removeChartView(chart);
            }
        }

        chartsArea.setChildOrder(children);
        chartsArea.layout();
        chartsArea.redraw();
    }

    @Override
    public boolean setFocus() {
        if (chartsArea != null)
            return chartsArea.setFocus();
        else
            return super.setFocus();
    }

    private static String getChartSheetName(ChartSheet chartSheet) {
        return StringUtils.defaultIfBlank(chartSheet.getName(), "<unnamed>");
    }

    @Override
    public boolean gotoObject(Object object) {
        if (object == chartSheet) {
            return true;
        }
        if (object instanceof EObject) {
            EObject eobject = (EObject)object;
            for (Chart chart : chartSheet.getCharts())
                if (chart == eobject) {
                    // TODO scroll to chart
                    return true;
                }
        }
        return false;
    }

    @Override
    public ChartViewer getActiveChartViewer() {
        ISelection selection = chartsArea.getSelection();
        if (selection instanceof IStructuredSelection) {
            IStructuredSelection structuredSelection = (IStructuredSelection)selection;
            if (structuredSelection.size() == 1 &&
                    structuredSelection.getFirstElement() instanceof ChartViewer)
                return (ChartViewer)structuredSelection.getFirstElement();
        }
        return null;
    }

    protected void handleChildOrderChanged() {
        List<Chart> charts = new ArrayList<Chart>();
        for (Control child : chartsArea.getChildren()) {
            if (child instanceof ChartViewer) {
                Chart chart = findChart(child);
                if (chart != null)
                    charts.add(chart);
            }
        }

        EditingDomain domain = scaveEditor.getEditingDomain();
        CompoundCommand command = new CompoundCommand("Move");
        EStructuralFeature feature = ScaveModelPackage.eINSTANCE.getChartSheet_Charts();
        command.append(SetCommand.create(domain, chartSheet, feature, charts));
        scaveEditor.executeCommand(command);
    }

    private void setChartSheetProperty(String name, String value) {
        if (ChartSheetProperties.PROP_COLUMN_COUNT.equals(name)) {
            GridLayout gridLayout = (GridLayout)chartsArea.getLayout();
            gridLayout.numColumns = value == null ? ChartSheetProperties.DEFAULT_COLUMN_COUNT : Integer.parseInt(value);
            updateLayout();
        }
        else if (ChartSheetProperties.PROP_MIN_CHART_WIDTH.equals(name)) {
            int minWidth = value == null ? ChartSheetProperties.DEFAULT_MIN_CHART_WIDTH : Integer.parseInt(value);
            for (Control child : chartsArea.getChildren())
                ((GridData)child.getLayoutData()).minimumWidth = minWidth;
            updateLayout();
        }
        else if (ChartSheetProperties.PROP_MIN_CHART_HEIGHT.equals(name)) {
            int minHeight = value == null ? ChartSheetProperties.DEFAULT_MIN_CHART_HEIGHT : Integer.parseInt(value);
            for (Control child : chartsArea.getChildren())
                ((GridData)child.getLayoutData()).minimumHeight = minHeight;
            updateLayout();
        }
        else if (ChartSheetProperties.PROP_DISPLAY_CHART_DETAILS.equals(name)) {
            boolean displayChartDetails = value == null ? ChartSheetProperties.DEFAULT_DISPLAY_CHART_DETAILS : Boolean.parseBoolean(value);
            for (Control child : chartsArea.getChildren())
                if (child instanceof ChartViewer)
                    setDisplayChartDetails((ChartViewer)child, displayChartDetails);
            updateLayout();
        }
    }

    private void setDisplayChartDetails(ChartViewer chartCanvas, boolean displayChartDetails) {
        chartCanvas.setDisplayTitle(displayChartDetails);
        chartCanvas.setDisplayLegendTooltip(displayChartDetails);
        chartCanvas.setDisplayAxesDetails(displayChartDetails);
    }

    private static class FillLayout extends Layout implements ILayoutExtension {
        @Override
        protected Point computeSize(Composite composite, int wHint, int hHint, boolean flushCache) {
            Rectangle rect = composite.getParent().getParent().getClientArea();
            Point location = composite.getLocation();
            Point size = new Point(rect.width - rect.x - location.x, rect.height - rect.y - location.y);

            for (Control child : composite.getChildren()) {
                Point childSize = child.computeSize(SWT.DEFAULT, SWT.DEFAULT, flushCache);
                size.x = Math.max(size.x, childSize.x);
                size.y = Math.max(size.y, childSize.y);
            }

            return size;
        }

        @Override
        protected void layout(Composite composite, boolean flushCache) {
            Rectangle rect = composite.getClientArea ();
            for (Control child : composite.getChildren())
                child.setBounds(rect);
        }

        public int computeMinimumWidth(Composite composite, boolean changed) {
            return 0;
        }

        public int computeMaximumWidth(Composite composite, boolean changed) {
            return computeSize(composite, SWT.DEFAULT, SWT.DEFAULT, false).x;
        }
    }

    private void updateLayout() {
        chartsArea.layout(true);
        scrolledComposite.setMinSize(chartsArea.computeSize(SWT.DEFAULT, SWT.DEFAULT));
    }
}
