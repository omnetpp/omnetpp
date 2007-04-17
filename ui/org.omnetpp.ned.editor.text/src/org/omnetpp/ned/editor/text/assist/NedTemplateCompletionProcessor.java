package org.omnetpp.ned.editor.text.assist;

import org.eclipse.jface.text.IRegion;
import org.eclipse.jface.text.ITextViewer;
import org.eclipse.jface.text.templates.Template;
import org.eclipse.jface.text.templates.TemplateContextType;
import org.omnetpp.common.editor.text.IncrementalCompletionProcessor;
import org.omnetpp.common.editor.text.NedCompletionHelper;
import org.omnetpp.ned.editor.text.TextualNedEditorPlugin;

public class NedTemplateCompletionProcessor extends IncrementalCompletionProcessor {
    
    protected Template[] getTemplates(String contextTypeId) {
        return TextualNedEditorPlugin.getDefault().getTemplateStore().getTemplates(contextTypeId);
    }

    protected TemplateContextType getContextType(ITextViewer viewer, IRegion region) {
        return TextualNedEditorPlugin.getDefault().getContextTypeRegistry().getContextType(NedCompletionHelper.DEFAULT_NED_CONTEXT_TYPE);
    }

}
