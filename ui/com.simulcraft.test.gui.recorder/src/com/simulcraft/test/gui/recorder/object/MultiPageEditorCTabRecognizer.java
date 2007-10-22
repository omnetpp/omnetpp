package com.simulcraft.test.gui.recorder.object;

import org.eclipse.swt.custom.CTabFolder;
import org.eclipse.swt.custom.CTabItem;
import org.eclipse.ui.IEditorReference;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.part.MultiPageEditorPart;
import org.omnetpp.common.util.ReflectionUtils;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaSequence;

public class MultiPageEditorCTabRecognizer extends ObjectRecognizer {
    public MultiPageEditorCTabRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaSequence identifyObject(Object uiObject) {
        if (uiObject instanceof CTabItem) {
            CTabItem item = (CTabItem)uiObject;
            MultiPageEditorPart editor = findMultiPageEditorOwnerOf(item);
            if (editor != null)
                return makeMethodCall(editor, expr("getCTabItem("+quoteLabel(item.getText())+")", 0.9, item));
        }
        return null;
    }

    protected MultiPageEditorPart findMultiPageEditorOwnerOf(CTabItem ctabItem) {
        CTabFolder ctabFolder = ctabItem.getParent();
        IWorkbenchWindow workbenchWindow = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
        for (IWorkbenchPage page : workbenchWindow.getPages()) {
            for (IEditorReference editorRef : page.getEditorReferences()) {
                if (editorRef.getEditor(false) instanceof MultiPageEditorPart) {
                    MultiPageEditorPart multiPageEditor = (MultiPageEditorPart)editorRef.getEditor(false);
                    if (ReflectionUtils.getFieldValue(multiPageEditor, "container") == ctabFolder)
                        return multiPageEditor;
                }
            }
        }
        return null;
    }
}

