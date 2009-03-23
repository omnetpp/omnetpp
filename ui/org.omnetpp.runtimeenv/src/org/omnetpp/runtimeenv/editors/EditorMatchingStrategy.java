package org.omnetpp.runtimeenv.editors;

import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorMatchingStrategy;
import org.eclipse.ui.IEditorReference;

//FIXME maybe not needed at all
public class EditorMatchingStrategy implements IEditorMatchingStrategy {
    @Override
    public boolean matches(IEditorReference editorRef, IEditorInput input) {
        if (input instanceof ModuleIDEditorInput && editorRef.getId().equals(ModelCanvas.EDITOR_ID))
            return true;
        return false;
    }

}
