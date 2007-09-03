package org.omnetpp.ned.editor.text.assist;

import java.util.Collections;
import java.util.List;

import org.eclipse.jface.text.ITextViewer;
import org.eclipse.jface.text.contentassist.ICompletionProposal;
import org.omnetpp.common.editor.text.NedCompletionHelper;
import org.omnetpp.common.editor.text.NedSyntaxHighlightHelper;

/**
 * Private NED documentation completion processor.
 *
 * @author rhornig
 */
public class NedPrivateDocCompletionProcessor extends NedTemplateCompletionProcessor {

	@Override
	@SuppressWarnings("unchecked")
	public ICompletionProposal[] computeCompletionProposals(ITextViewer viewer, int documentOffset) {
        List<ICompletionProposal> result = 
            createProposals(viewer, documentOffset, NedSyntaxHighlightHelper.nedWordDetector,
                    "", NedCompletionHelper.proposedPrivateDocTodo, " ", null);

        Collections.sort(result, CompletionProposalComparator.getInstance());
        return result.toArray(new ICompletionProposal[result.size()]);
    } 

}
