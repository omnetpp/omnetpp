package com.swtworkbench.community.xswt.editors;

import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.editors.text.TextEditor;

import com.swtworkbench.community.xswt.XswtPlugin;
import com.swtworkbench.community.xswt.editor.views.XSWTPreview;

public class XSWTEditor extends TextEditor {
    private ColorManager colorManager;

    public XSWTEditor() {
        this.colorManager = new ColorManager();
        setSourceViewerConfiguration(new XMLConfiguration(this.colorManager));
        setDocumentProvider(new XMLDocumentProvider());
    }

    @Override
    public void init(IEditorSite site, IEditorInput input) throws PartInitException {
        super.init(site, input);
        
        // open the "XSWTPreview" view
        try {
            IWorkbenchPage workbenchPage = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage();
            if (workbenchPage != null)   // note: may be null during platform startup...
                workbenchPage.showView(XSWTPreview.VIEW_ID);
        } catch (Exception e) {
            XswtPlugin.logError(e);
        }
    }
    
    public void dispose() {
        this.colorManager.dispose();
        super.dispose();
    }
}