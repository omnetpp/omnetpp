package org.omnetpp.ide;

import org.eclipse.ui.IFolderLayout;
import org.eclipse.ui.IPageLayout;
import org.eclipse.ui.IPerspectiveFactory;
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
public class PerspectiveFactory implements IPerspectiveFactory {

	public void createInitialLayout(IPageLayout layout) {
		// Note: starting point of this code was JDT's "Java" perspective,
		// see JavaPerspectiveFactory (org.eclipse.jdt.internal.ui package)

 		String editorArea = layout.getEditorArea();

 		IFolderLayout folder = layout.createFolder("left", IPageLayout.LEFT, (float)0.25, editorArea); //$NON-NLS-1$
		folder.addView(IConstants.PROJECT_EXPLORER_ID);
        IFolderLayout blfolder = layout.createFolder("leftbottom", IPageLayout.BOTTOM, (float)0.50, "left"); //$NON-NLS-1$
        blfolder.addView(IPageLayout.ID_PROP_SHEET);
        blfolder.addView(IPageLayout.ID_OUTLINE);

		IFolderLayout outputfolder = layout.createFolder("bottom", IPageLayout.BOTTOM, (float)0.75, editorArea); //$NON-NLS-1$
		outputfolder.addView(IConstants.NEWS_VIEW_ID);
		outputfolder.addView(IPageLayout.ID_PROBLEM_VIEW);
		outputfolder.addView(IConstants.MODULEHIERARCHY_VIEW_ID);
		outputfolder.addView(IConstants.MODULEPARAMETERS_VIEW_ID);
		outputfolder.addView(IConstants.NEDINHERITANCE_VIEW_ID);
		
		// note: placeholders for our other views don't need to be listed here,
		// because they are contributed via perspectiveExtension extensions
		// in their plugin.xml's.
		outputfolder.addPlaceholder(IPageLayout.ID_TASK_LIST);
		outputfolder.addPlaceholder(IPageLayout.ID_BOOKMARKS);
		outputfolder.addPlaceholder(IProgressConstants.PROGRESS_VIEW_ID);

		// actionsets
		layout.addActionSet(IPageLayout.ID_NAVIGATE_ACTION_SET);
		layout.addActionSet("org.eclipse.debug.ui.launchActionSet"); // IDebugUIConstants.LAUNCH_ACTION_SET

		// views - standard workbench
		layout.addShowViewShortcut(IPageLayout.ID_OUTLINE);
		layout.addShowViewShortcut(IPageLayout.ID_PROP_SHEET);
		layout.addShowViewShortcut(IPageLayout.ID_PROBLEM_VIEW);
		layout.addShowViewShortcut(IPageLayout.ID_RES_NAV);
		layout.addShowViewShortcut(IConstants.PROJECT_EXPLORER_ID);
		layout.addShowViewShortcut(IPageLayout.ID_TASK_LIST);
		layout.addShowViewShortcut(IProgressConstants.PROGRESS_VIEW_ID);
		layout.addShowViewShortcut(IConstants.MODULEPARAMETERS_VIEW_ID);
		layout.addShowViewShortcut(IConstants.MODULEHIERARCHY_VIEW_ID);
		layout.addShowViewShortcut(IConstants.NEDINHERITANCE_VIEW_ID);
		layout.addShowViewShortcut(IConstants.DATASET_VIEW_ID);
		layout.addShowViewShortcut(IConstants.VECTORBROWSER_VIEW_ID);
		layout.addShowViewShortcut(IConstants.SEQUENCECHART_VIEW_ID);
		layout.addShowViewShortcut(IConstants.EVENTLOG_VIEW_ID);
		layout.addShowViewShortcut(IConstants.NEWS_VIEW_ID);


        // TODO final strings needed here
		// new actions - our wizards
		layout.addNewWizardShortcut("org.omnetpp.ide.wizard.newProjectWizard");//$NON-NLS-1$
        layout.addNewWizardShortcut("org.omnetpp.cdt.wizard.omnetppCCProjectWizard");//$NON-NLS-1$
        layout.addNewWizardShortcut(IConstants.NEW_SIMPLE_MODULE_WIZARD_ID);
        layout.addNewWizardShortcut(IConstants.NEW_COMPOUND_MODULE_WIZARD_ID);
        layout.addNewWizardShortcut(IConstants.NEW_NETWORK_WIZARD_ID);
		layout.addNewWizardShortcut("org.omnetpp.ned.editor.wizard.new.file");//$NON-NLS-1$
		layout.addNewWizardShortcut("org.omnetpp.msg.editor.wizard.new.file");//$NON-NLS-1$
		layout.addNewWizardShortcut("org.omnetpp.inifile.editor.wizards.NewInifileWizard");//$NON-NLS-1$
		layout.addNewWizardShortcut("org.omnetpp.scave.wizard.ScaveModelModelWizardID"); //$NON-NLS-1$
		// standard platform wizards
		layout.addNewWizardShortcut("org.eclipse.ui.wizards.new.folder");//$NON-NLS-1$
		layout.addNewWizardShortcut("org.eclipse.ui.wizards.new.file");//$NON-NLS-1$
		layout.addNewWizardShortcut("org.eclipse.ui.editors.wizards.UntitledTextFileWizard");//$NON-NLS-1$
	}

}
