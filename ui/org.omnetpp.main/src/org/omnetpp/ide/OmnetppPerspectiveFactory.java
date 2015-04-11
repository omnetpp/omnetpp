/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ide;

import org.eclipse.ui.IFolderLayout;
import org.eclipse.ui.IPageLayout;
import org.eclipse.ui.IPerspectiveFactory;
import org.eclipse.ui.console.IConsoleConstants;
import org.eclipse.ui.progress.IProgressConstants;
import org.omnetpp.common.IConstants;

/**
 * The default OMNeT++ perspective. We mainly define our own perspective
 * in order to contribute our New File wizards to the File menu.
 *
 * Note: all of this would be possible from plugin.xml too, using
 * the <code>org.eclipse.ui.perspectiveExtensions</code> extension.
 *
 * @author Andras
 */
public class OmnetppPerspectiveFactory implements IPerspectiveFactory {

    public void createInitialLayout(IPageLayout layout) {
        // Note: starting point of this code was JDT's "Java" perspective,
        // see JavaPerspectiveFactory (org.eclipse.jdt.internal.ui package)

        String editorArea = layout.getEditorArea();

        IFolderLayout leftFolder = layout.createFolder("left", IPageLayout.LEFT, (float)0.25, editorArea); //$NON-NLS-1$
        leftFolder.addView(IPageLayout.ID_PROJECT_EXPLORER);
        leftFolder.addPlaceholder(IPageLayout.ID_RES_NAV);

        IFolderLayout leftBottomFolder = layout.createFolder("leftbottom", IPageLayout.BOTTOM, (float)0.50, "left"); //$NON-NLS-1$
        leftBottomFolder.addView(IPageLayout.ID_PROP_SHEET);
        leftBottomFolder.addView(IPageLayout.ID_OUTLINE);

        IFolderLayout bottomFolder = layout.createFolder("bottom", IPageLayout.BOTTOM, (float)0.75, editorArea); //$NON-NLS-1$
        bottomFolder.addView(IPageLayout.ID_PROBLEM_VIEW);
        bottomFolder.addView(IConstants.MODULEHIERARCHY_VIEW_ID);
        bottomFolder.addView(IConstants.MODULEPARAMETERS_VIEW_ID);
        bottomFolder.addView(IConstants.NEDINHERITANCE_VIEW_ID);

        // note: placeholders for our other views don't need to be listed here,
        // because they are contributed via perspectiveExtension extensions
        // in their plugin.xml's.
        bottomFolder.addPlaceholder(IConstants.NEW_VERSION_VIEW_ID);
        bottomFolder.addPlaceholder(IPageLayout.ID_TASK_LIST);
        bottomFolder.addPlaceholder(IPageLayout.ID_BOOKMARKS);
        bottomFolder.addPlaceholder(IConsoleConstants.ID_CONSOLE_VIEW);
        bottomFolder.addPlaceholder(IProgressConstants.PROGRESS_VIEW_ID);

        // actionsets
        layout.addActionSet(IPageLayout.ID_NAVIGATE_ACTION_SET);
        layout.addActionSet("org.eclipse.debug.ui.launchActionSet"); // IDebugUIConstants.LAUNCH_ACTION_SET

        // views - standard workbench
        layout.addShowViewShortcut(IPageLayout.ID_OUTLINE);
        layout.addShowViewShortcut(IPageLayout.ID_PROP_SHEET);
        layout.addShowViewShortcut(IPageLayout.ID_PROBLEM_VIEW);
        layout.addShowViewShortcut(IPageLayout.ID_PROJECT_EXPLORER);
        layout.addShowViewShortcut(IPageLayout.ID_TASK_LIST);
        layout.addShowViewShortcut(IProgressConstants.PROGRESS_VIEW_ID);
        layout.addShowViewShortcut(IConstants.MODULEPARAMETERS_VIEW_ID);
        layout.addShowViewShortcut(IConstants.MODULEHIERARCHY_VIEW_ID);
        layout.addShowViewShortcut(IConstants.NEDINHERITANCE_VIEW_ID);
        layout.addShowViewShortcut(IConstants.DATASET_VIEW_ID);
        layout.addShowViewShortcut(IConstants.VECTORBROWSER_VIEW_ID);
        layout.addShowViewShortcut(IConstants.SEQUENCECHART_VIEW_ID);
        layout.addShowViewShortcut(IConstants.EVENTLOG_VIEW_ID);

        // new actions - our wizards
        layout.addNewWizardShortcut(IConstants.NEW_OMNETPP_PROJECT_WIZARD_ID);
        layout.addNewWizardShortcut(IConstants.NEW_OMNETPP_CC_PROJECT_WIZARD_ID);
        layout.addNewWizardShortcut(IConstants.NEW_SIMULATION_WIZARD_ID);
        layout.addNewWizardShortcut(IConstants.NEW_SIMPLE_MODULE_WIZARD_ID);
        layout.addNewWizardShortcut(IConstants.NEW_COMPOUND_MODULE_WIZARD_ID);
        layout.addNewWizardShortcut(IConstants.NEW_NETWORK_WIZARD_ID);
        layout.addNewWizardShortcut(IConstants.NEW_NEDFILE_WIZARD_ID);
        layout.addNewWizardShortcut(IConstants.NEW_MSGFILE_WIZARD_ID);
        layout.addNewWizardShortcut(IConstants.NEW_INIFILE_WIZARD_ID);
        layout.addNewWizardShortcut(IConstants.NEW_SCAVEFILE_WIZARD_ID);
        // standard platform wizards
        layout.addNewWizardShortcut("org.eclipse.ui.wizards.new.folder");//$NON-NLS-1$
        layout.addNewWizardShortcut("org.eclipse.ui.wizards.new.file");//$NON-NLS-1$
        layout.addNewWizardShortcut("org.eclipse.ui.editors.wizards.UntitledTextFileWizard");//$NON-NLS-1$
        // wizard wizard
        layout.addNewWizardShortcut(IConstants.NEW_WIZARD_WIZARD_ID); //$NON-NLS-1$
    }

}
