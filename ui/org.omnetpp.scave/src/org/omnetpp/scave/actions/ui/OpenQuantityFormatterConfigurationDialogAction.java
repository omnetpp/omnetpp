/*--------------------------------------------------------------*
  Copyright (C) 2006-2022 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions.ui;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.actions.AbstractScaveAction;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.ui.QuantityFormatterRegistry;
import org.omnetpp.scave.editors.ui.QunatityFormatterConfigurationDialog;

public class OpenQuantityFormatterConfigurationDialogAction extends AbstractScaveAction {
    public OpenQuantityFormatterConfigurationDialogAction() {
        setText("Configure Number Format");
        setImageDescriptor(ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_COGWHEEL));
    }

    @Override
    protected void doRun(ScaveEditor scaveEditor, ISelection selection) throws CoreException {
        QunatityFormatterConfigurationDialog dialog = new QunatityFormatterConfigurationDialog(Display.getCurrent().getActiveShell());
        dialog.setContentProvider(new ArrayContentProvider());
        dialog.setMappings(QuantityFormatterRegistry.getInstance().getMappings());
        dialog.open();
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        return true;
    }
}