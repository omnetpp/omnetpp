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

import org.omnetpp.scave.engine.ScalarResult;
import org.omnetpp.scave.plugin.ScavePlugin;

public class ScalarTableLabelProvider implements ITableLabelProvider, ScalarPanelConstants
{
    public Image getColumnImage(Object element, int columnIndex) {
        return null;
    }

    public String getColumnText(Object element, int columnIndex) {
        long id = ((Long)element).longValue();
        ScalarResult d = ScavePlugin.getDefault().resultFileManager.getScalar(id);
        switch (columnIndex)
        {
        case COL_DIRECTORY: return d.getRun().getFile().getDirectory();
        case COL_FILENAME: return d.getRun().getFile().getFileName();
        case COL_RUN: return d.getRun().getRunName();
        case COL_MODULE: return d.getModuleName();
        case COL_NAME: return d.getName();
        case COL_VALUE: return ""+d.getValue();
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