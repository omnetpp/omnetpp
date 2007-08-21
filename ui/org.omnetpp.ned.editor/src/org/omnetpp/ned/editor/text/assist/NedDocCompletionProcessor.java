package org.omnetpp.ned.editor.text.assist;

import java.util.List;

import org.eclipse.jface.text.ITextViewer;
import org.eclipse.jface.text.contentassist.ICompletionProposal;
import org.omnetpp.common.editor.text.NedCompletionHelper;



/**
 * NED documentation completion processor.
 *
 * @author rhornig
 */
public class NedDocCompletionProcessor extends NedTemplateCompletionProcessor {

    @Override
	public ICompletionProposal[] computeCompletionProposals(ITextViewer viewer, int documentOffset) {
        List<ICompletionProposal> result = 
            createProposals(viewer, documentOffset, NedCompletionHelper.nedAtWordDetector,
                    "@", NedCompletionHelper.proposedDocKeywords, " ", null);

        result.addAll(createProposals(viewer, documentOffset, NedCompletionHelper.nedDocTagDetector, 
                "<", NedCompletionHelper.proposedDocTags, "> ", null));
        result.addAll(createProposals(viewer, documentOffset, NedCompletionHelper.nedDocTagDetector, 
                "</", NedCompletionHelper.proposedDocTags, "> ", null));
        
//        Collections.sort(result, CompletionProposalComparator.getInstance());
        return result.toArray(new ICompletionProposal[result.size()]);
    } 

    @Override
	public char[] getCompletionProposalAutoActivationCharacters() {
        return new char[] { '@', '<' };
    }
    
}
