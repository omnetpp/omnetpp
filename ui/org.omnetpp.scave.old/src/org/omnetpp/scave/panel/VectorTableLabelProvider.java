/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

/**
 *
 */
package org.omnetpp.scave.panel;

import org.eclipse.jface.viewers.ILabelProviderListener;
import org.eclipse.jface.viewers.ITableLabelProvider;
import org.eclipse.swt.graphics.Image;

import org.omnetpp.scave.engine.VectorResult;
import org.omnetpp.scave.plugin.ScavePlugin;

public class VectorTableLabelProvider implements ITableLabelProvider, VectorPanelConstants
{
    public Image getColumnImage(Object element, int columnIndex) {
        return null;
    }

    public String getColumnText(Object element, int columnIndex) {
        long id = ((Long)element).longValue();
        VectorResult vec = ScavePlugin.getDefault().resultFileManager.getVector(id);
        switch (columnIndex)
        {
        case COL_DIRECTORY: return vec.getRun().getFile().getDirectory();
        case COL_FILENAME: return vec.getRun().getFile().getFileName();
        case COL_MODULE: return vec.getModuleName();
        case COL_NAME: return vec.getName();
        case COL_LENGTH: return "not counted";
        case COL_MEAN: return "not yet";
        case COL_STDDEV: return "not yet";
        default: return "n/a";
        }
    }

    public void addListener(ILabelProviderListener listener) {
    }

    public void dispose() {
    }

    public boolean isLabelProperty(Object element, String property) {
        return false;
    }

    public void removeListener(ILabelProviderListener listener) {
    }
}