/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.forms;

import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.scave.model.ModelObject;
import org.omnetpp.scave.model.InputFile;

/**
 * Edit form of input files.
 *
 * @author tomi
 */
public class InputFileEditForm extends BaseScaveObjectEditForm {

    /**
     * The edited input file.
     */
    //private InputFile inputFile;

    // edit controls of the features
    private Text nameText;

    public InputFileEditForm(InputFile inputFile, ModelObject parent) {
        super(inputFile);
        //this.inputFile = inputFile;
    }

    /**
     * Add edit controls the panel.
     */
    public void populatePanel(Composite panel) {
        panel.setLayout(new GridLayout(2, false));
        Label label = new Label(panel, SWT.NONE);
        label.setLayoutData(new GridData());
        label.setText("File name:");
        nameText = new Text(panel, SWT.BORDER);
        nameText.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
    }

}
