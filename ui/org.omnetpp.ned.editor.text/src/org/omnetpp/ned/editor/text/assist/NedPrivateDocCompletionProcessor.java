package org.omnetpp.ned.editor.text.assist;

import java.util.Collections;
import java.util.List;

import org.eclipse.jface.text.ITextViewer;
import org.eclipse.jface.text.contentassist.ICompletionProposal;
import org.omnetpp.ned.editor.text.NedHelper;

/**
 * Private NED documentation completion processor.
 */
public class NedPrivateDocCompletionProcessor extends IncrementalCompletionProcessor {
    public ICompletionProposal[] computeCompletionProposals(ITextViewer viewer, int documentOffset) {
        List result = 
            createProposals(viewer, documentOffset, NedHelper.nedWordDetector,
                    "", NedHelper.proposedPrivateDocTodo, " ", null);

        Collections.sort(result, CompletionProposalComparator.getInstance());
        return (ICompletionProposal[]) result.toArray(new ICompletionProposal[result.size()]);
    } 

}
