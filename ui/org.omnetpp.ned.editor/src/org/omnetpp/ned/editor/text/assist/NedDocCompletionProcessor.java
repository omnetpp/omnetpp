package org.omnetpp.ned.editor.text.assist;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.jface.text.ITextViewer;
import org.eclipse.jface.text.contentassist.ICompletionProposal;
import org.omnetpp.common.editor.text.NedCompletionHelper;
import org.omnetpp.common.editor.text.NedSyntaxHighlightHelper;



/**
 * NED documentation completion processor.
 *
 * @author rhornig
 */
public class NedDocCompletionProcessor extends NedTemplateCompletionProcessor {

    @Override
	public ICompletionProposal[] computeCompletionProposals(ITextViewer viewer, int documentOffset) {
        List<ICompletionProposal> result = new ArrayList<ICompletionProposal>();
        
        result.addAll(createProposals(viewer, documentOffset, new NedSyntaxHighlightHelper.NedAtWordDetector(),
        		"@", NedCompletionHelper.proposedDocKeywords, " ", null));
        result.addAll(createProposals(viewer, documentOffset, new NedSyntaxHighlightHelper.NedDocTagDetector(), 
                "<", NedCompletionHelper.proposedDocTags, "> ", null));
        result.addAll(createProposals(viewer, documentOffset, new NedSyntaxHighlightHelper.NedDocTagDetector(), 
                "</", NedCompletionHelper.proposedDocTags, "> ", null));
        
//        Collections.sort(result, CompletionProposalComparator.getInstance());
        return result.toArray(new ICompletionProposal[result.size()]);
    } 

    @Override
	public char[] getCompletionProposalAutoActivationCharacters() {
        return new char[] { '@', '<' };
    }
    
}
