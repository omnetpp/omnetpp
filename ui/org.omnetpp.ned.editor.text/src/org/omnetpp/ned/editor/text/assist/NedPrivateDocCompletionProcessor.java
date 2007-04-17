package org.omnetpp.ned.editor.text.assist;

import java.util.Collections;
import java.util.List;

import org.eclipse.jface.text.ITextViewer;
import org.eclipse.jface.text.contentassist.ICompletionProposal;
import org.omnetpp.common.editor.text.NedCompletionHelper;

/**
 * Private NED documentation completion processor.
 */
public class NedPrivateDocCompletionProcessor extends NedTemplateCompletionProcessor {

	@SuppressWarnings("unchecked")
	public ICompletionProposal[] computeCompletionProposals(ITextViewer viewer, int documentOffset) {
        List<ICompletionProposal> result = 
            createProposals(viewer, documentOffset, NedCompletionHelper.nedWordDetector,
                    "", NedCompletionHelper.proposedPrivateDocTodo, " ", null);

        Collections.sort(result, CompletionProposalComparator.getInstance());
        return (ICompletionProposal[]) result.toArray(new ICompletionProposal[result.size()]);
    } 

}
