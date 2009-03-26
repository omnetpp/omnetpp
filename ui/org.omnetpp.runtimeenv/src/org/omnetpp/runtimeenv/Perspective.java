package org.omnetpp.runtimeenv;

import org.eclipse.ui.IFolderLayout;
import org.eclipse.ui.IPageLayout;
import org.eclipse.ui.IPerspectiveFactory;
import org.omnetpp.runtimeenv.views.ModuleOutputView;
import org.omnetpp.runtimeenv.views.ObjectPropertiesView;
import org.omnetpp.runtimeenv.views.ObjectTreeView;

public class Perspective implements IPerspectiveFactory {

	public void createInitialLayout(IPageLayout layout) {
		String editorArea = layout.getEditorArea();
		layout.setEditorAreaVisible(true);
		layout.setFixed(false);
		
        IFolderLayout leftFolder = layout.createFolder("left", IPageLayout.LEFT, 0.25f, editorArea); //$NON-NLS-1$
        leftFolder.addView(ObjectTreeView.ID);
        
        IFolderLayout leftBottomFolder = layout.createFolder("leftbottom", IPageLayout.BOTTOM, 0.50f, "left"); //$NON-NLS-1$
        leftBottomFolder.addView(ObjectPropertiesView.ID);

        IFolderLayout bottomFolder = layout.createFolder("bottom", IPageLayout.BOTTOM, 0.75f, editorArea); //$NON-NLS-1$
        bottomFolder.addView(ModuleOutputView.ID);
	}

}
