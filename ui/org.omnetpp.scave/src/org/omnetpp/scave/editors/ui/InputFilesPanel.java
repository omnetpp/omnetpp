/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.ui;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.forms.widgets.FormToolkit;

public class InputFilesPanel extends TreeWithButtonsPanel {

    private Button addFileButton;
    private Button addWildcardButton;
    private Button removeFileButton;
    private Button editButton;

    public InputFilesPanel(Composite parent, int style) {
        super(parent, style);
        initialize();
    }

    private void initialize() {
        Composite buttonPanel = getButtonPanel();
        FormToolkit toolkit = getFormToolkit();
        addFileButton = toolkit.createButton(buttonPanel, "Add...", SWT.NONE);
        addWildcardButton = toolkit.createButton(buttonPanel, "Add wildcard...", SWT.NONE);
        editButton = toolkit.createButton(buttonPanel, "Edit", SWT.NONE);
        removeFileButton = toolkit.createButton(buttonPanel, "Remove", SWT.NONE);
    }

    public Button getAddFileButton() {
        return addFileButton;
    }

    public Button getAddWildcardButton() {
        return addWildcardButton;
    }

    public Button getEditButton() {
        return editButton;
    }

    public Button getRemoveFileButton() {
        return removeFileButton;
    }
}
