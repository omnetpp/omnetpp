package org.omnetpp.common.editor.text;


import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.resource.ImageRegistry;
import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.IRegion;
import org.eclipse.jface.text.ITextSelection;
import org.eclipse.jface.text.ITextViewer;
import org.eclipse.jface.text.contentassist.CompletionProposal;
import org.eclipse.jface.text.contentassist.ICompletionProposal;
import org.eclipse.jface.text.rules.IWordDetector;
import org.eclipse.jface.text.templates.SimpleTemplateVariableResolver;
import org.eclipse.jface.text.templates.Template;
import org.eclipse.jface.text.templates.TemplateCompletionProcessor;
import org.eclipse.jface.text.templates.TemplateContext;
import org.eclipse.jface.text.templates.TemplateException;
import org.eclipse.jface.text.templates.TemplateProposal;
import org.eclipse.swt.graphics.Image;

import org.omnetpp.common.CommonPlugin;
import org.omnetpp.common.util.StringUtils;

/**
 * Generic incremental type completion processor.
 * @author rhornig
 */
public abstract class IncrementalCompletionProcessor extends TemplateCompletionProcessor {
    private static final String DEFAULT_IMAGE = "icons/obj16/template.png"; 

    public class  IndentTemplateVariableResolver extends SimpleTemplateVariableResolver {
        protected IndentTemplateVariableResolver(String value) {
            super("indent", "Indentation template variable");
            setEvaluationString(value);
        }
    }
    
    /**
     * Helper comparator class to compare CompletionProposals using relevance and the the display name 
     */
    @SuppressWarnings("unchecked")
	protected static class CompletionProposalComparator implements Comparator {
        private static CompletionProposalComparator instance = null;
        
        public static CompletionProposalComparator getInstance() {
            if (instance == null)
                instance = new CompletionProposalComparator();
            return instance;
        }
        
        public int compare(Object arg0, Object arg1) {
           
            // first order according to the relevance
            if (arg0 instanceof TemplateProposal && arg1 instanceof TemplateProposal) {
                int compRes = ((TemplateProposal) arg1).getRelevance() - ((TemplateProposal) arg0).getRelevance();
                if (compRes != 0) return compRes;
                return ((TemplateProposal)arg0).getDisplayString().compareToIgnoreCase(
                        ((TemplateProposal)arg1).getDisplayString());
            }
            // if relevance is the same compare using display names
            return ((ICompletionProposal)arg0).getDisplayString().compareToIgnoreCase(
                        ((ICompletionProposal)arg1).getDisplayString());
        }
    }

    /**
     * Create a List of ICompletionProposal from an array of string. Checks the word under the current cursor
     * position and filters the proposal accordingly.
     */
    protected List<ICompletionProposal> createProposals(ITextViewer viewer, int documentOffset, IWordDetector wordDetector, String startStr, String[] proposalString, String endStr, String description) {
        String descriptions[] = new String[proposalString.length]; 
        Arrays.fill(descriptions, description);
        return createProposals(viewer, documentOffset, wordDetector, startStr, proposalString, endStr, descriptions);
        
    }

    /**
     * Create a List of ICompletionProposal from an array of string. Checks the word under the current cursor
     * position and filters the proposal accordingly.
     */
    protected List<ICompletionProposal> createProposals(ITextViewer viewer, int documentOffset, IWordDetector wordDetector, String startStr, String[] proposalString, String endStr, String[] descriptions) {
        List<ICompletionProposal> propList = new ArrayList<ICompletionProposal>();
        String prefix;
        IRegion wordRegion;
        try {
        	wordRegion = TextEditorUtil.detectWordRegion(viewer, documentOffset, wordDetector); 
            prefix = viewer.getDocument().get(wordRegion.getOffset(), documentOffset - wordRegion.getOffset());
        } catch (BadLocationException e) { 
        	CommonPlugin.logError(e);
        	return propList;
        }

        // we have to sort the name and the description together so we merge them in a single string
        String SEPARATOR = "\u0000";  // ASCII 0
        String displayLine[] = new String[proposalString.length];
        for (int i=0; i<proposalString.length; i++) {
            Assert.isTrue(!proposalString[i].contains(SEPARATOR), "Proposal string contains an internal terminator char.");
            displayLine[i] = proposalString[i]+SEPARATOR+StringUtils.nullToEmpty(descriptions[i]);
        }
        
        Arrays.sort(displayLine, StringUtils.dictionaryComparator);
        
        for (int i = 0 ;i < displayLine.length; ++i) {
            String prop = startStr + StringUtils.substringBefore(displayLine[i], SEPARATOR) + endStr;
            String descr = StringUtils.substringAfter(displayLine[i], SEPARATOR);
            if (prop.toLowerCase().startsWith(prefix.toLowerCase())) {
            	String displayText = StringUtils.isEmpty(descr) ? StringUtils.strip(prop) : StringUtils.strip(prop)+" - "+descr;
                propList.add(new CompletionProposal(prop, wordRegion.getOffset(), wordRegion.getLength(), prop.length(), null, displayText, null, null));
            }
        }

        return propList;
    }

	/**
	 * This method is necessary because TemplateCompletionProcessor.computeCompletionProposals()
	 * doesn't let us specify what templates we want to add, but insists on calling
	 * getTemplates() instead. This is a copy of that computeCompletionProposals(), with
	 * Template[] added to the arg list. 
	 *
	 * @author andras
	 * @see org.eclipse.jface.text.contentassist.IContentAssistProcessor#computeCompletionProposals(org.eclipse.jface.text.ITextViewer, int)
	 */
	@SuppressWarnings("unchecked")
	public ICompletionProposal[] createTemplateProposals(ITextViewer viewer, int offset, IWordDetector wordDetector, Template[] templates) {

		ITextSelection selection= (ITextSelection) viewer.getSelectionProvider().getSelection();

		// adjust offset to end of normalized selection
		if (selection.getOffset() == offset)
			offset= selection.getOffset() + selection.getLength();

        IRegion wordRegion; 
        String prefix;
        try {
            wordRegion = TextEditorUtil.detectWordRegion(viewer, offset, wordDetector); 
            prefix = viewer.getDocument().get(wordRegion.getOffset(), offset - wordRegion.getOffset());
        } catch (BadLocationException e) {
        	CommonPlugin.logError(e);
			return new ICompletionProposal[0];
        }

		TemplateContext context= createContext(viewer, wordRegion);
        // set the current indentation in a variable so we will be able to use ${indent} in templates
        // ${indent} is implicitly added after each \n char during the creation of template proposals
        String indentPrefix = "";
        try {
            int lineStartOffset = viewer.getDocument().getLineInformationOfOffset(offset).getOffset();
            // get the line's first part (till the beginning of the prefix string) and replace tabs with
            // 4 spaces
            indentPrefix = viewer.getDocument().get(lineStartOffset, wordRegion.getOffset()-lineStartOffset).replace("\t", "    ");
            // turn it int spaces only with the same length
            indentPrefix = StringUtils.repeat(" ", indentPrefix.length());
        } catch (BadLocationException e1) { }
        context.getContextType().addResolver(new IndentTemplateVariableResolver(indentPrefix));
        
		if (context == null)
			return new ICompletionProposal[0];

		context.setVariable("selection", selection.getText()); // name of the selection variables {line, word}_selection //$NON-NLS-1$

		List<ICompletionProposal> matches = new ArrayList<ICompletionProposal>();
		for (int i= 0; i < templates.length; i++) {
			Template template= templates[i];
			try {
				context.getContextType().validate(template.getPattern());
			} catch (TemplateException e) {
				continue;
			}
			if (template.matches(prefix, context.getContextType().getId()) && template.getName().startsWith(prefix))
				matches.add(createProposal(template, context, wordRegion, getRelevance(template, prefix)));
		}

		Collections.sort(matches, CompletionProposalComparator.getInstance());

		return (ICompletionProposal[]) matches.toArray(new ICompletionProposal[matches.size()]);
	}
    
    /**
     * Always return the default image.
     * 
     * @param template the template, ignored in this implementation
     * @return the default template image
     */
    protected Image getImage(Template template) {
        ImageRegistry registry = CommonPlugin.getDefault().getImageRegistry();
        Image image = registry.get(DEFAULT_IMAGE);
        if (image == null) {
            ImageDescriptor desc = CommonPlugin.imageDescriptorFromPlugin(CommonPlugin.PLUGIN_ID, DEFAULT_IMAGE);
            registry.put(DEFAULT_IMAGE, desc);
            image = registry.get(DEFAULT_IMAGE);
        }
        return image;
    }
}
