/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.viewers.ISelection;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ScaveEditor;

/**
 * TODO
 *
 * @author andras
 */
public class DecreaseDecimalPlacesAction extends AbstractScaveAction {
    private static final int MIN_PRECISION = 1;

    public DecreaseDecimalPlacesAction() {
        setText("Decrease Display Precision");
        setImageDescriptor(ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_DECIMALS_DEC));
    }

    @Override
    protected void doRun(ScaveEditor scaveEditor, ISelection selection) throws CoreException {
        int prec = scaveEditor.getBrowseDataPage().getNumericPrecision();
        if (prec > MIN_PRECISION)
            scaveEditor.getBrowseDataPage().setNumericPrecision(prec - 1);
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        int prec = editor.getBrowseDataPage().getNumericPrecision();
        return prec > MIN_PRECISION;
    }
}
