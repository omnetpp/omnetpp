/*--------------------------------------------------------------*
  Copyright (C) 2006-2022 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions.ui;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.preference.IPreferenceNode;
import org.eclipse.jface.preference.PreferenceDialog;
import org.eclipse.jface.preference.PreferenceManager;
import org.eclipse.jface.preference.PreferenceNode;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.actions.AbstractScaveAction;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.preferences.QuantityFormattersPreferencePage;

public class OpenQuantityFormatterConfigurationDialogAction extends AbstractScaveAction {
    public OpenQuantityFormatterConfigurationDialogAction() {
        setText("Configure Number Format");
        setImageDescriptor(ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_COGWHEEL));
    }

    @Override
    protected void doRun(ScaveEditor scaveEditor, ISelection selection) throws CoreException {
        // code based on MemoryViewPrefAction

        PreferenceManager prefManager = new PreferenceManager();

        QuantityFormattersPreferencePage page = new QuantityFormattersPreferencePage();
        page.init(PlatformUI.getWorkbench()); // normally done by WorkbenchPreferenceDialog
        IPreferenceNode node = new PreferenceNode("org.omnetpp.scave.preferences.QuantityFormattersPreferencePage", page); //$NON-NLS-1$
        prefManager.addToRoot(node);

        PreferenceDialog dialog = new PreferenceDialog(Display.getCurrent().getActiveShell(), prefManager);
        dialog.create();
        dialog.open();
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        return true;
    }
}