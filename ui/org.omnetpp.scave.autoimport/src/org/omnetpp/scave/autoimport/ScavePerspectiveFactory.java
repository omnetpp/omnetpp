/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.autoimport;

import org.eclipse.ui.IFolderLayout;
import org.eclipse.ui.IPageLayout;
import org.eclipse.ui.IPerspectiveFactory;
import org.omnetpp.common.IConstants;

/**
 * The default Scave perspective.
 * @author Andras
 */
public class ScavePerspectiveFactory implements IPerspectiveFactory {

    public void createInitialLayout(IPageLayout layout) {
        String editorArea = layout.getEditorArea();

        IFolderLayout leftFolder = layout.createFolder("left", IPageLayout.LEFT, (float)0.25, editorArea); //$NON-NLS-1$
        leftFolder.addView(IPageLayout.ID_PROJECT_EXPLORER);
        leftFolder.addPlaceholder(IPageLayout.ID_RES_NAV);

        IFolderLayout leftBottomFolder = layout.createFolder("leftbottom", IPageLayout.BOTTOM, (float)0.50, "left"); //$NON-NLS-1$
        leftBottomFolder.addView(IPageLayout.ID_PROP_SHEET);
        leftBottomFolder.addView(IPageLayout.ID_OUTLINE);

        IFolderLayout bottomFolder = layout.createFolder("bottom", IPageLayout.BOTTOM, (float)0.75, editorArea); //$NON-NLS-1$
        bottomFolder.addView(IConstants.VECTORBROWSER_VIEW_ID);
        bottomFolder.addView(IConstants.DATASET_VIEW_ID);
        bottomFolder.addView(IPageLayout.ID_PROBLEM_VIEW);

        // note: placeholders for our other views don't need to be listed here,
        // because they are contributed via perspectiveExtension extensions
        // in their plugin.xml's.

        // actionsets
        layout.addActionSet(IPageLayout.ID_NAVIGATE_ACTION_SET);
        layout.addActionSet("org.eclipse.debug.ui.launchActionSet"); // IDebugUIConstants.LAUNCH_ACTION_SET

        // views - standard workbench
        layout.addShowViewShortcut(IPageLayout.ID_OUTLINE);
        layout.addShowViewShortcut(IPageLayout.ID_PROP_SHEET);
        layout.addShowViewShortcut(IPageLayout.ID_PROJECT_EXPLORER);
        layout.addShowViewShortcut(IConstants.DATASET_VIEW_ID);
        layout.addShowViewShortcut(IConstants.VECTORBROWSER_VIEW_ID);

        // new actions - our wizards
        layout.addNewWizardShortcut(IConstants.NEW_SCAVEFILE_WIZARD_ID);
        layout.addNewWizardShortcut("org.eclipse.ui.wizards.new.folder");//$NON-NLS-1$
        layout.addNewWizardShortcut("org.eclipse.ui.wizards.new.file");//$NON-NLS-1$
        layout.addNewWizardShortcut("org.eclipse.ui.editors.wizards.UntitledTextFileWizard");//$NON-NLS-1$
    }

}
