/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.properties.util;

import org.eclipse.jface.viewers.DialogCellEditor;

/**
 * Provides a dialog cell editor that allows the editing of several properties
 *
 * @author rhornig
 */
public interface IDialogCellEditorProvider {

    /**
     * Returns a dialog editor that allows the editing of the property source.
     * DelegatingPropertySource uses this editor for editing
     */
    DialogCellEditor getCellEditor();

}
