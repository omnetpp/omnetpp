package org.omnetpp.ned.editor.text.assist;

import java.util.List;

import org.eclipse.jface.text.ITextViewer;
import org.eclipse.jface.text.contentassist.ICompletionProposal;
import org.omnetpp.common.editor.text.NedTextEditorHelper;



/**
 * NED documentation completion processor.
 */
public class NedDocCompletionProcessor extends NedTemplateCompletionProcessor {

    public ICompletionProposal[] computeCompletionProposals(ITextViewer viewer, int documentOffset) {
        List<ICompletionProposal> result = 
            createProposals(viewer, documentOffset, NedTextEditorHelper.nedAtWordDetector,
                    "@", NedTextEditorHelper.proposedDocKeywords, " ", null);

        result.addAll(createProposals(viewer, documentOffset, NedTextEditorHelper.nedDocTagDetector, 
                "<", NedTextEditorHelper.proposedDocTags, "> ", null));
        result.addAll(createProposals(viewer, documentOffset, NedTextEditorHelper.nedDocTagDetector, 
                "</", NedTextEditorHelper.proposedDocTags, "> ", null));
        
//        Collections.sort(result, CompletionProposalComparator.getInstance());
        return (ICompletionProposal[]) result.toArray(new ICompletionProposal[result.size()]);
    } 

    public char[] getCompletionProposalAutoActivationCharacters() {
        return new char[] { '@', '<' };
    }
    
}
