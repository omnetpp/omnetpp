package com.simulcraft.test.gui.recorder.object;

import org.eclipse.core.runtime.Assert;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IViewPart;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.part.MultiPageEditorPart;
import org.omnetpp.common.util.IPredicate;

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
                MultiPageEditorPart parentMultiPageEditor = findParentMultiPageEditorFor(workbenchPart);
                if (parentMultiPageEditor != null) {
                    String label = "FIXME-FIXME-FIXME";
                    return makeSeq(parentMultiPageEditor, expr("getActivePageEditor("+quoteLabel(label)+")", 0.9, workbenchPart));
                }
            }
            
            // identify by title
            IWorkbenchWindow workbenchWindow = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
            if (workbenchPart instanceof IEditorPart)
                return makeSeq(workbenchWindow, expr("findEditorPartByTitle("+quoteLabel(workbenchPart.getTitle())+")", 0.9, workbenchPart));
            else if (workbenchPart instanceof IViewPart)
                return makeSeq(workbenchWindow, expr("findViewPartByTitle("+quoteLabel(workbenchPart.getTitle())+")", 0.9, workbenchPart));
            else if (workbenchPart != null)
                Assert.isTrue(false, "Unknown workbenchpart: " + workbenchPart);
        }
        return null;
    }

    protected MultiPageEditorPart findParentMultiPageEditorFor(final IWorkbenchPart workbenchPart) {
        return (MultiPageEditorPart) findWorkbenchPart(false, new IPredicate() {
            public boolean matches(Object object) {
                if (object instanceof MultiPageEditorPart && containsPageEditor(((MultiPageEditorPart)object), workbenchPart))
                    return true;
                return false;
            }
        });
    }

    protected boolean containsPageEditor(MultiPageEditorPart multiPageEditor, final IWorkbenchPart workbenchPart) {
        return findPageEditor(multiPageEditor, new IPredicate() {
            public boolean matches(Object object) {
                return object == workbenchPart;
            }
        }) != null;
    }

}