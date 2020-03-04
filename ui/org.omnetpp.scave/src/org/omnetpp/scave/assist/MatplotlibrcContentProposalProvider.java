package org.omnetpp.scave.assist;

import org.eclipse.jface.fieldassist.IContentProposal;
import org.eclipse.jface.fieldassist.IContentProposalProvider;
import org.omnetpp.common.contentassist.ContentProposalEx;

public class MatplotlibrcContentProposalProvider implements IContentProposalProvider {

    @Override
    public IContentProposal[] getProposals(String contents, int position) {
        // return new IContentProposal[] { new ContentProposalEx("Hello!", "greeting", "Just a greeting") };
        return null;
    }
}

