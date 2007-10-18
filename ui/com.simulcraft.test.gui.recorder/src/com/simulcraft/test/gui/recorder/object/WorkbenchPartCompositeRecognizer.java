package com.simulcraft.test.gui.recorder.object;

import org.eclipse.core.runtime.Assert;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IEditorReference;
import org.eclipse.ui.IViewPart;
import org.eclipse.ui.IViewReference;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.IWorkbenchPartSite;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.internal.PartSite;
import org.eclipse.ui.part.MultiPageEditorSite;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaSequence;


public class WorkbenchPartCompositeRecognizer extends ObjectRecognizer {
    public WorkbenchPartCompositeRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaSequence identifyObject(Object uiObject) {
        if (uiObject instanceof Composite) {
            Composite composite = (Composite)uiObject;

            IWorkbenchWindow workbenchWindow = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
            IWorkbenchPart workbenchPart = getCorrespondingWorkbenchPart(composite);
            if (workbenchPart instanceof IEditorPart)
                return makeSeq(workbenchWindow, expr("findEditorPartByTitle("+quote(workbenchPart.getTitle())+").getComposite()", 0.9, composite));
            else if (workbenchPart instanceof IViewPart)
                return makeSeq(workbenchWindow, expr("findViewPartByTitle("+quote(workbenchPart.getTitle())+").getComposite()", 0.9, composite));
            else if (workbenchPart != null)
                Assert.isTrue(false, "Unknown workbenchpart: " + workbenchPart);
        }
        return null;
    }

    protected IWorkbenchPart getCorrespondingWorkbenchPart(Composite composite) {
        IWorkbenchWindow workbenchWindow = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
        for (IWorkbenchPage page : workbenchWindow.getPages()) {
            for (IViewReference viewReference : page.getViewReferences())
                if (getComposite(viewReference.getPart(false)) == composite)
                    return viewReference.getPart(false);
            for (IEditorReference editorReference : page.getEditorReferences())
                if (getComposite(editorReference.getPart(false)) == composite)
                    return editorReference.getPart(false);
        }
        return null;
    }
    
    @SuppressWarnings("restriction")
    protected Composite getComposite(IWorkbenchPart workbenchPart) {
        if (workbenchPart == null)
            return null;
        
        IWorkbenchPartSite site = workbenchPart.getSite();
        if (site instanceof PartSite)
            return (Composite)((PartSite)site).getPane().getControl();
        else if (site instanceof MultiPageEditorSite)
            return (Composite)((PartSite)((MultiPageEditorSite)site).getMultiPageEditor().getSite()).getPane().getControl();
        Assert.isTrue(false, "Unknown workbenchpart site: " + site);
        return null;
    }

}