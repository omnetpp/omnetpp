package org.omnetpp.scave.editors;

import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IPersistableElement;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.scave.model.Chart;

class ChartScriptEditorInput implements IEditorInput {

    Chart chart;

    public ChartScriptEditorInput(Chart chart) {
        this.chart = chart;
    }

    @Override
    public <T> T getAdapter(Class<T> adapter) {
        // System.out.println("getAdapter: " + adapter.getName());
        return null; // ScaveEditor.this.getEditorInput().getAdapter(adapter);
    }

    @Override
    public boolean exists() {
        // TODO Auto-generated method stub
        return true;
    }

    @Override
    public ImageDescriptor getImageDescriptor() {
        // TODO Auto-generated method stub
        return ImageFactory.global().getDescriptor(ImageFactory.DEFAULT);
    }

    @Override
    public String getName() {
        return chart.getName();
    }

    @Override
    public IPersistableElement getPersistable() {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public String getToolTipText() {
        // TODO Auto-generated method stub
        return "tooltip";
    }

}