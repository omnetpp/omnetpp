package org.omnetpp.common.editor;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.Assert;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.part.FileEditorInput;

/**
 * Utility methods for managing editors
 * @author rhornig
 */
public class EditorUtil {
    public final static String MULTIPAGE_NEDEDITOR_ID = "org.omnetpp.ned.editor";
    
    /**
     * Opens a NED file in text mode and sets a highlight range on it
     * @param file
     * @param startLine
     * @param endLine
     */
    public static void openNEDEditor(IFile file, int startLine, int endLine) {
        try {
            IFileEditorInput fileEditorInput = new FileEditorInput(file);
            IEditorPart editor = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage()
                    .openEditor(fileEditorInput, EditorUtil.MULTIPAGE_NEDEDITOR_ID, true);

            // select the component so it will be visible in the opened editor
            if (editor instanceof ISelectionSupport)
                ((ISelectionSupport)editor).setTextHighlightRange(startLine, endLine);

        } catch (PartInitException e) {
            // should not happen
            e.printStackTrace();  //FIXME log it or something. Just "print" is not OK!
            Assert.isTrue(false);
        }
    }

}
