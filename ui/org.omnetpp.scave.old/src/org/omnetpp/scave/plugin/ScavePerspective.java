/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.plugin;

import org.eclipse.ui.IFolderLayout;
import org.eclipse.ui.IPageLayout;
import org.eclipse.ui.IPerspectiveFactory;

/**
 */
public class ScavePerspective implements IPerspectiveFactory {
    /**
     * Constructs a new layout engine.
     */
    public ScavePerspective() {
        super();
    }

    /**
     * Defines the initial layout for a perspective.
     *
     * @param layout the factory used to add views to the perspective
     */
    public void createInitialLayout(IPageLayout layout) {
        layout.addPerspectiveShortcut("org.omnetpp.scave.ScavePerspective");
        defineActions(layout);
        defineLayout(layout);
    }

    /**
     * Defines the initial actions for a page.
     * @param layout The layout we are filling
     */
    public void defineActions(IPageLayout layout) {
        // Add "new wizards".
        layout.addNewWizardShortcut("org.eclipse.ui.wizards.new.folder");//$NON-NLS-1$
        layout.addNewWizardShortcut("org.eclipse.ui.wizards.new.file");//$NON-NLS-1$

        // Add "show views".
        layout.addShowViewShortcut(IPageLayout.ID_RES_NAV);
        //layout.addShowViewShortcut(IPageLayout.ID_BOOKMARKS);
        layout.addShowViewShortcut(IPageLayout.ID_OUTLINE);
        layout.addShowViewShortcut(IPageLayout.ID_PROP_SHEET);
        //layout.addShowViewShortcut(IPageLayout.ID_PROBLEM_VIEW);
        //layout.addShowViewShortcut(IPageLayout.ID_TASK_LIST);

        layout.addActionSet(IPageLayout.ID_NAVIGATE_ACTION_SET);
    }

    /**
     * Defines the initial layout for a page.
     * @param layout The layout we are filling
     */
    public void defineLayout(IPageLayout layout) {
        // Editors are placed for free.
        String editorArea = layout.getEditorArea();

        // Top left.
        IFolderLayout topLeft = layout.createFolder(
                "topLeft", IPageLayout.LEFT, (float) 0.26, editorArea);//$NON-NLS-1$
        topLeft.addView(IPageLayout.ID_RES_NAV);
        //topLeft.addPlaceholder(IPageLayout.ID_BOOKMARKS);

        // Bottom left.
        IFolderLayout bottomLeft = layout.createFolder(
                "bottomLeft", IPageLayout.BOTTOM, (float) 0.50,//$NON-NLS-1$
                "topLeft");//$NON-NLS-1$
        bottomLeft.addView(IPageLayout.ID_OUTLINE);

        // Bottom right.
        //IFolderLayout bottomRight = layout.createFolder(
        //        "bottomRight", IPageLayout.BOTTOM, (float) 0.66,//$NON-NLS-1$
        //        editorArea);
        //
        //bottomRight.addView(IPageLayout.ID_TASK_LIST);
    }
}
