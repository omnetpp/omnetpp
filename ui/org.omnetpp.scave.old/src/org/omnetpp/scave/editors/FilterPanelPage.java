/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors;

import java.util.Map;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.swt.SWT;
import org.eclipse.swt.dnd.DND;
import org.eclipse.swt.dnd.DragSource;
import org.eclipse.swt.dnd.DragSourceAdapter;
import org.eclipse.swt.dnd.DragSourceEvent;
import org.eclipse.swt.dnd.Transfer;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.omnetpp.common.xml.XMLWriter;
import org.omnetpp.scave.model.IDListIO;
import org.omnetpp.scave.model.IDListModel;
import org.omnetpp.scave.model.IDListTransfer;
import org.omnetpp.scave.model.IDatasetStrategy;
import org.omnetpp.scave.model.IModelChangeListener;
import org.omnetpp.scave.panel.FilterPanel;
import org.xml.sax.ContentHandler;

public class FilterPanelPage implements IDatasetEditorPage {

    private DatasetEditor editor;

    /** Allows us common handling of scalars and vectors */
    private IDatasetStrategy strategy;

    /** The Data panel that appears on the first page */
    private FilterPanel filterPanel;

    public FilterPanelPage(IDatasetStrategy strategy) {
        this.strategy = strategy;
    }

    public String getPageTitle() {
        return "Data";
    }

    public FilterPanel getFilterPanel() {
        return filterPanel;
    }

    public IDListModel getDataset() {
        return editor.getDataset();
    }

    public void setEditor(DatasetEditor editor) {
        this.editor = editor;
    }

    public void init() {
    }

    public void dispose() {
        filterPanel.dispose();
    }

    public Control createPageControl(Composite parent) {
        filterPanel = strategy.createFilterPanel(parent, SWT.NONE);
        filterPanel.setStatusLineManager(editor.getStatusLineManager());
        filterPanel.setDataset(getDataset());
        setupDragSource(filterPanel.getPanel().getTable());
        editor.setupDropTarget(filterPanel.getPanel());
        return filterPanel.getPanel();
    }

    public void finalizePage() {
        // add change listener that updates the dirty flag
        getDataset().addListener(new IModelChangeListener() {
            public void handleChange() {
                editor.markDirty();
            }
        });
        filterPanel.refresh();
    }

    public void activate() {
    }

    public void deactivate() {
    }

    public void save(XMLWriter writer, IProgressMonitor progressMonitor) {
        IDListIO.save(getDataset().get(), writer, progressMonitor);
    }

    public Map<String, ContentHandler> getLoader(IProgressMonitor progressMonitor) {
        return IDListIO.getContentHandlers(getDataset().get(), progressMonitor);
    }

    protected void setupDragSource(Control dragSourceControl) {
        // make table d&d source for IDLists
        DragSource dragSrc = new DragSource(dragSourceControl, DND.DROP_DEFAULT | DND.DROP_COPY);
        dragSrc.setTransfer(new Transfer[] { IDListTransfer.getInstance() });
        dragSrc.addDragListener(new DragSourceAdapter() {
            public void dragSetData(DragSourceEvent event) {
                event.data = filterPanel.getSelection();
            }
        });
    }
}
