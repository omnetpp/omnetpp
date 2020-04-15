/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.text.assist;

import java.util.Collections;
import java.util.List;

import org.eclipse.jface.text.ITextViewer;
import org.eclipse.jface.text.contentassist.ICompletionProposal;
import org.omnetpp.common.editor.text.NedCompletionHelper;
import org.omnetpp.common.editor.text.SyntaxHighlightHelper;

/**
 * Private NED documentation completion processor.
 *
 * @author rhornig
 */
public class NedPrivateDocCompletionProcessor extends NedTemplateCompletionProcessor {

    @Override
    public ICompletionProposal[] computeCompletionProposals(ITextViewer viewer, int documentOffset) {
        List<ICompletionProposal> result =
            createProposals(viewer, documentOffset, new SyntaxHighlightHelper.NedWordDetector(),
                    "", NedCompletionHelper.proposedPrivateDocTodo, " ", "");

        Collections.sort(result, CompletionProposalComparator.getInstance());
        return result.toArray(new ICompletionProposal[result.size()]);
    }

}
