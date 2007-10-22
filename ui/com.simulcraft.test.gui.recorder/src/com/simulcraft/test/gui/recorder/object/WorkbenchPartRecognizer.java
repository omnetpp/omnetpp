package com.simulcraft.test.gui.recorder.object;

import org.eclipse.core.runtime.Assert;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IViewPart;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.part.MultiPageEditorPart;
import org.omnetpp.common.util.IPredicate;
import org.omnetpp.common.util.ReflectionUtils;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaSequence;


public class WorkbenchPartRecognizer extends ObjectRecognizer {
    public WorkbenchPartRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaSequence identifyObject(Object uiObject) {
        if (uiObject instanceof IWorkbenchPart) {
            final IWorkbenchPart workbenchPart = (IWorkbenchPart)uiObject;
            
            // if editor is in a multipage editor, identify it like that
            if (workbenchPart instanceof IEditorPart) {
                MultiPageEditorPart parentMultiPageEditor = findParentMultiPageEditorFor((IEditorPart)workbenchPart);
                if (parentMultiPageEditor != null) {
                    String label = findPageEditorLabel(parentMultiPageEditor, (IEditorPart)workbenchPart);
                    return makeMethodCall(parentMultiPageEditor, expr("getPageEditor("+quoteLabel(label)+")", 0.9, workbenchPart));
                }
            }
            
            // identify by title
            IWorkbenchWindow workbenchWindow = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
            if (workbenchPart instanceof IEditorPart)
                return makeMethodCall(workbenchWindow, expr("findEditorPartByTitle("+quoteLabel(workbenchPart.getTitle())+")", 0.9, workbenchPart));
            else if (workbenchPart instanceof IViewPart)
                return makeMethodCall(workbenchWindow, expr("findViewPartByTitle("+quoteLabel(workbenchPart.getTitle())+")", 0.9, workbenchPart));
            else if (workbenchPart != null)
                Assert.isTrue(false, "Unknown workbenchpart: " + workbenchPart);
        }
        return null;
    }

    protected MultiPageEditorPart findParentMultiPageEditorFor(final IEditorPart editorPart) {
        return (MultiPageEditorPart) findWorkbenchPart(false, new IPredicate() {
            public boolean matches(Object object) {
                if (object instanceof MultiPageEditorPart && findPageEditorLabel(((MultiPageEditorPart)object), editorPart) != null)
                    return true;
                return false;
            }
        });
    }

    protected String findPageEditorLabel(MultiPageEditorPart multiPageEditor, IEditorPart pageEditor) {
        int numPages = (Integer) ReflectionUtils.invokeMethod(multiPageEditor, "getPageCount");
        for (int i=0; i<numPages; i++) {
            IEditorPart part = (IEditorPart) ReflectionUtils.invokeMethod(multiPageEditor, "getEditor", i);
            if (part == pageEditor)
                return (String) ReflectionUtils.invokeMethod(multiPageEditor, "getPageText", i);
        }
        return null;
    }

}