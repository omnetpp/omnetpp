/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.jface.viewers.ISelectionProvider;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.ui.views.properties.IPropertySheetPage;
import org.eclipse.ui.views.properties.IPropertySource;
import org.eclipse.ui.views.properties.IPropertySourceProvider;
import org.eclipse.ui.views.properties.PropertySheetPage;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.event.ChartChangeEvent;
import org.jfree.chart.event.ChartChangeListener;
import org.omnetpp.common.properties.PropertySource;
import org.omnetpp.common.properties.PropertySourceIO;
import org.omnetpp.common.ui.SelectionProvider;
import org.omnetpp.common.xml.XMLWriter;
import org.omnetpp.scave.charting.InteractiveChart;
import org.omnetpp.scave.model.IDListModel;
import org.omnetpp.scave.model.IDatasetStrategy;
import org.omnetpp.scave.model.IModelChangeListener;
import org.xml.sax.ContentHandler;

public class ChartPage implements IDatasetEditorPage, IAdaptable {

    /** Allows us common handling of scalars and vectors */
    private IDatasetStrategy strategy;

    private DatasetEditor editor;

    /** The (optional) chart page only gets updated if (when) visible */
    private boolean chartNeedsUpdate = false;

    private boolean isActive;

    /** The Chart which appears on the page */
    private InteractiveChart chartWrapper;
    private JFreeChart chart;

    private ISelectionProvider selectionProvider = new SelectionProvider();


    public ChartPage(IDatasetStrategy strategy) {
        this.strategy = strategy;
    }

    public String getPageTitle() {
        return "Chart";
    }

    public IDListModel getDataset() {
        return editor.getDataset();
    }

    public void setEditor(DatasetEditor editor) {
        this.editor = editor;
    }

    public void init() {
        editor.getSite().setSelectionProvider(selectionProvider);
    }

    public void dispose() {
        editor.getSite().setSelectionProvider(null);
    }

    public Control createPageControl(Composite parent) {
        Composite panel = new Composite(parent, SWT.NONE);
        panel.setLayout(new GridLayout(2,false));
        //int index = addPage(panel);

        chartWrapper = new InteractiveChart(panel, SWT.NONE);
        chartWrapper.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        Button refreshButton = new Button(panel, SWT.NONE);
        refreshButton.setLayoutData(new GridData(SWT.RIGHT, SWT.TOP, false, false));
        refreshButton.setText("Refresh");
        refreshButton.addListener(SWT.Selection, new Listener() {
            public void handleEvent(Event event) {
                chartWrapper.refresh();
            }
        });

        chart = strategy.createEmptyChart();
        return panel;
    }

    public void finalizePage() {
        editor.setupDropTarget(chartWrapper);
        chartWrapper.setChart(chart);
        strategy.updateDataset(chart, getDataset().get());

        chart.addChangeListener(new ChartChangeListener() {
            public void chartChanged(ChartChangeEvent event) {
                editor.markDirty();
            }
        });

        getDataset().addListener(new IModelChangeListener() {
            public void handleChange() {
                if (isActive)
                    updateChart();
                else
                    chartNeedsUpdate = true;
            }
        });
    }

    public void activate() {
        isActive = true;
        selectionProvider.setSelection(new StructuredSelection(chartWrapper));
        if (chartNeedsUpdate)
            updateChart();
    }

    public void deactivate() {
        isActive = false;
        selectionProvider.setSelection(StructuredSelection.EMPTY);
    }

    public void updateChart() {
        if (chartWrapper.getChart()!=null) {
            strategy.updateDataset(chartWrapper.getChart(), getDataset().get());
            chartWrapper.refresh();
        }
        chartNeedsUpdate = false;
    }



    public Object getAdapter(Class required) {
        if (required == IPropertySheetPage.class)
        {
            PropertySheetPage page =  new PropertySheetPage();
            page.setPropertySourceProvider(new IPropertySourceProvider() {
                public IPropertySource getPropertySource(Object object) {
                    if (object instanceof IPropertySource)
                        return (IPropertySource)object;
                    else if (object == chartWrapper)
                        return new ChartProperties(chartWrapper);
                    else
                        return null;
                }
            });
            return page;
        }

        return null;
    }

    public void save(XMLWriter writer, IProgressMonitor progressMonitor) throws IOException {
        if (chartWrapper != null) {
            writer.writeStartElement("chart");
            PropertySource propertySource = new ChartProperties(chartWrapper);
            PropertySourceIO.save(propertySource, writer);
            writer.writeEndElement("chart");
        }
    }

    public Map<String, ContentHandler> getLoader(IProgressMonitor progressMonitor) {
        Map<String,ContentHandler> handlers = new HashMap<String,ContentHandler>(2);

        PropertySourceIO.SAXHandler chartHandler =
            new PropertySourceIO.SAXHandler(new ChartProperties(chart));
        handlers.put("chart", chartHandler);
        return handlers;
    }
}
