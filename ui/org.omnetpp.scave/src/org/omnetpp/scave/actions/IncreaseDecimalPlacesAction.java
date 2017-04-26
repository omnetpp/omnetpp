/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ScaveEditor;

/**
 * TODO
 *
 * @author andras
 */
public class IncreaseDecimalPlacesAction extends AbstractScaveAction {
    private static final int MAX_PRECISION = 19;  // max number of digits in int64

    public IncreaseDecimalPlacesAction() {
        setText("Increase Display Precision");
        setImageDescriptor(ScavePlugin.getImageDescriptor("icons/full/etool16/decimals_inc.png"));
    }

    @Override
    protected void doRun(ScaveEditor scaveEditor, IStructuredSelection selection) {
        int prec = scaveEditor.getBrowseDataPage().getNumericPrecision();
        if (prec < MAX_PRECISION)
            scaveEditor.getBrowseDataPage().setNumericPrecision(prec + 1);
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
        int prec = editor.getBrowseDataPage().getNumericPrecision();
        return prec < MAX_PRECISION;
    }

}
