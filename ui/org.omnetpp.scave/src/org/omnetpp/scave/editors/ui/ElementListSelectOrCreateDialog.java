/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.ui;

import org.eclipse.jface.resource.JFaceResources;
import org.eclipse.jface.viewers.ILabelProvider;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.dialogs.ElementListSelectionDialog;

public class ElementListSelectOrCreateDialog extends ElementListSelectionDialog {

    protected Button newButton;
    protected ICallback callback;
    protected Object[] elements; //Note: duplicate of super.fElements and fFilteredList.fElements, as they are not accessible

    public interface ICallback {
        public Object createNewObject(String currentFilter);
    }

    public ElementListSelectOrCreateDialog(Shell parent, ILabelProvider renderer) {
        super(parent, renderer);
    }

    @Override
    protected Control createDialogArea(Composite parent) {
        Composite contents = (Composite) super.createDialogArea(parent);

        newButton = new Button(contents, SWT.PUSH);
        newButton.setText("New");
        newButton.setFont(JFaceResources.getDialogFont());
        setButtonLayoutData(newButton);
        ((GridData)newButton.getLayoutData()).horizontalAlignment = SWT.RIGHT;

        newButton.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent event) {
                Object newObject = callback.createNewObject(getFilter());
                if (newObject != null)
                    addToDialog(newObject);
            }
        });

        return contents;
    }

    @Override
    public void setElements(Object[] elements) {
        // Note: this method is only overridden because elements[] is not accessible later
        // (inaccessible field and missing getter in both ElementListSelectionDialog and FilteredList)
        this.elements = elements;
        super.setElements(elements);
    }

    protected void addToDialog(Object element) {
        Object[] tmp = new Object[elements.length + 1];
        System.arraycopy(elements, 0, tmp, 0, elements.length);
        tmp[elements.length] = element;
        setElements(tmp);
        setListElements(elements);
        setSelection(new Object[] {element});
    }

    public Button getNewButton() {
        return newButton;
    }

    public void setCallback(ICallback callback) {
        this.callback = callback;
    }
}
