/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors;

import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IPersistableElement;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.scave.model.Chart;

/**
 * IEditorInput for ChartScriptEditor.
 */
class ChartScriptEditorInput implements IEditorInput {
    private Chart chart;

    public ChartScriptEditorInput(Chart chart) {
        this.chart = chart;
    }

    @Override
    public <T> T getAdapter(Class<T> adapter) {
        return null;
    }

    @Override
    public boolean exists() {
        return true;
    }

    public Chart getChart() {
        return chart;
    }

    @Override
    public ImageDescriptor getImageDescriptor() {
        return ImageFactory.global().getDescriptor(ImageFactory.DEFAULT);
    }

    @Override
    public String getName() {
        return chart.getName();
    }

    @Override
    public IPersistableElement getPersistable() {
        return null;
    }

    @Override
    public String getToolTipText() {
        return "id=" + chart.getId();
    }

    @Override
    public boolean equals(Object other) {
        return other != null && other instanceof ChartScriptEditorInput && ((ChartScriptEditorInput)other).chart == chart;
    }

}