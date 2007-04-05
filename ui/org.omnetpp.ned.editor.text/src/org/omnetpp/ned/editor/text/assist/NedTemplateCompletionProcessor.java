package org.omnetpp.ned.editor.text.assist;

import org.eclipse.jface.text.IRegion;
import org.eclipse.jface.text.ITextViewer;
import org.eclipse.jface.text.templates.Template;
import org.eclipse.jface.text.templates.TemplateContextType;
import org.omnetpp.common.editor.text.IncrementalCompletionProcessor;
import org.omnetpp.ned.editor.text.TextualNedEditorPlugin;

public class NedTemplateCompletionProcessor extends IncrementalCompletionProcessor {
    /**
     * Returns the all templates for this contextTypeId.
     * 
     * @param contextTypeId the context type
     * @return all templates
     */
    protected Template[] getTemplates(String contextTypeId) {
        return TextualNedEditorPlugin.getDefault().getTemplateStore().getTemplates(contextTypeId);
    }

    /**
     * Return the XML context type that is supported by this plug-in.
     * 
     * @param viewer the viewer, ignored in this implementation
     * @param region the region, ignored in this implementation
     * @return the supported XML context type
     */
    protected TemplateContextType getContextType(ITextViewer viewer, IRegion region) {
        return TextualNedEditorPlugin.getDefault().getContextTypeRegistry().getContextType(NedContextType.DEFAULT_CONTEXT_TYPE);
    }

}
