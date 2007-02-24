package org.omnetpp.ide;

import org.eclipse.ui.IFolderLayout;
import org.eclipse.ui.IPageLayout;
import org.eclipse.ui.IPerspectiveFactory;
import org.eclipse.ui.progress.IProgressConstants;

/**
 * The default OMNeT++/OMNEST perspective. We mainly define our own perspective 
 * in order to contribute our New File wizards to the File menu.
 * 
 * @author Andras
 */
public class PerspectiveFactory implements IPerspectiveFactory {

	public void createInitialLayout(IPageLayout layout) {
		// Note: starting point of this code was JDT's "Java" perspective,
		// see JavaPerspectiveFactory (org.eclipse.jdt.internal.ui package) 
		
 		String editorArea = layout.getEditorArea();
		
 		IFolderLayout folder = layout.createFolder("left", IPageLayout.LEFT, (float)0.33, editorArea); //$NON-NLS-1$
		folder.addView(IPageLayout.ID_RES_NAV);
		layout.addView(IPageLayout.ID_OUTLINE, IPageLayout.BOTTOM, (float)0.75, IPageLayout.ID_RES_NAV);
		
		IFolderLayout outputfolder = layout.createFolder("bottom", IPageLayout.BOTTOM, (float)0.75, editorArea); //$NON-NLS-1$
		outputfolder.addView(IPageLayout.ID_PROBLEM_VIEW);
		outputfolder.addView(IPageLayout.ID_PROP_SHEET);
		outputfolder.addView(IPageLayout.ID_TASK_LIST);
		outputfolder.addPlaceholder(IPageLayout.ID_BOOKMARKS);
		outputfolder.addPlaceholder(IProgressConstants.PROGRESS_VIEW_ID);
//XXX JDT stuff commented out
//		outputfolder.addPlaceholder(NewSearchUI.SEARCH_VIEW_ID);
//		outputfolder.addPlaceholder(IConsoleConstants.ID_CONSOLE_VIEW);
		
		// actionsets
//		layout.addActionSet(IDebugUIConstants.LAUNCH_ACTION_SET);
//		layout.addActionSet(JavaUI.ID_ACTION_SET);
//		layout.addActionSet(JavaUI.ID_ELEMENT_CREATION_ACTION_SET);
		layout.addActionSet(IPageLayout.ID_NAVIGATE_ACTION_SET);
		
//		// views - java
//		layout.addShowViewShortcut(JavaUI.ID_PACKAGES);
//		layout.addShowViewShortcut(JavaUI.ID_TYPE_HIERARCHY);
//		layout.addShowViewShortcut(JavaUI.ID_SOURCE_VIEW);
//		layout.addShowViewShortcut(JavaUI.ID_JAVADOC_VIEW);
//
//		// views - search
//		layout.addShowViewShortcut(NewSearchUI.SEARCH_VIEW_ID);
//		
//		// views - debugging
//		layout.addShowViewShortcut(IConsoleConstants.ID_CONSOLE_VIEW);

		// views - standard workbench
		layout.addShowViewShortcut(IPageLayout.ID_OUTLINE);
		layout.addShowViewShortcut(IPageLayout.ID_PROBLEM_VIEW);
		layout.addShowViewShortcut(IPageLayout.ID_RES_NAV);
		layout.addShowViewShortcut(IPageLayout.ID_TASK_LIST);
		layout.addShowViewShortcut(IProgressConstants.PROGRESS_VIEW_ID);
				
		// new actions - our wizards
		layout.addNewWizardShortcut("org.omnetpp.ned.editor.wizard.new.file");//$NON-NLS-1$
		layout.addNewWizardShortcut("org.omnetpp.inifile.editor.wizards.NewInifileWizard");//$NON-NLS-1$
		layout.addNewWizardShortcut("org.omnetpp.scave.wizard.ScaveModelModelWizardID"); //$NON-NLS-1$

		// standard platform wizards
		layout.addNewWizardShortcut("org.eclipse.ui.wizards.new.folder");//$NON-NLS-1$
		layout.addNewWizardShortcut("org.eclipse.ui.wizards.new.file");//$NON-NLS-1$
		layout.addNewWizardShortcut("org.eclipse.ui.editors.wizards.UntitledTextFileWizard");//$NON-NLS-1$
	}

}
