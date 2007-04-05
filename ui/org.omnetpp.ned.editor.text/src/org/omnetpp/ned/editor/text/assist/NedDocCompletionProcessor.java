package org.omnetpp.ned.editor.text.assist;

import java.util.List;

import org.eclipse.jface.text.ITextViewer;
import org.eclipse.jface.text.contentassist.ICompletionProposal;
import org.omnetpp.ned.editor.text.NedHelper;



/**
 * NED documentation completion processor.
 */
public class NedDocCompletionProcessor extends NedTemplateCompletionProcessor {

    public ICompletionProposal[] computeCompletionProposals(ITextViewer viewer, int documentOffset) {
        List<ICompletionProposal> result = 
            createProposals(viewer, documentOffset, NedHelper.nedAtWordDetector,
                    "@", NedHelper.proposedDocKeywords, " ", null);

        result.addAll(createProposals(viewer, documentOffset, NedHelper.nedDocTagDetector, 
                "<", NedHelper.proposedDocTags, "> ", null));
        result.addAll(createProposals(viewer, documentOffset, NedHelper.nedDocTagDetector, 
                "</", NedHelper.proposedDocTags, "> ", null));
        
//        Collections.sort(result, CompletionProposalComparator.getInstance());
        return (ICompletionProposal[]) result.toArray(new ICompletionProposal[result.size()]);
    } 

    public char[] getCompletionProposalAutoActivationCharacters() {
        return new char[] { '@', '<' };
    }
    
}
