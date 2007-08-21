package org.omnetpp.ned.editor.text.assist;

import org.eclipse.jface.text.IRegion;
import org.eclipse.jface.text.ITextViewer;
import org.eclipse.jface.text.templates.Template;
import org.eclipse.jface.text.templates.TemplateContextType;
import org.omnetpp.common.editor.text.IncrementalCompletionProcessor;
import org.omnetpp.common.editor.text.NedCompletionHelper;
import org.omnetpp.ned.editor.text.TextualNedEditor;

/**
 * Get the custom templates from the plugin's preference store
 *
 * @author rhornig
 */
public class NedTemplateCompletionProcessor extends IncrementalCompletionProcessor {
    
    @Override
	protected Template[] getTemplates(String contextTypeId) {
        return TextualNedEditor.getTemplateStore().getTemplates(contextTypeId);
    }

    @Override
	protected TemplateContextType getContextType(ITextViewer viewer, IRegion region) {
        return TextualNedEditor.getContextTypeRegistry().getContextType(NedCompletionHelper.DEFAULT_NED_CONTEXT_TYPE);
    }

}
