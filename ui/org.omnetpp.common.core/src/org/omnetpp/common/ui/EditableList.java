/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.ui;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Table;

/**
 * Table control with Add, Edit and Remove buttons, for editing a list of strings.
 * The labels, Add/Edit dialog titles and messages, input validation etc are all
 * customizable.
 *
 * @author Levy
 */
public class EditableList extends AbstractEditableList {
    /**
     * Creates the compound control. Style refers to the composite that contains all controls of the widget.
     */
    public EditableList(Composite parent, int style) {
        super(parent, style);
    }

    @Override
    protected Table createTable() {
        return new Table(this, SWT.MULTI | SWT.BORDER | SWT.V_SCROLL | SWT.H_SCROLL);
    }
}
